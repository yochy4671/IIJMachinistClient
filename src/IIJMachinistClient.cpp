/*
    IIJMachinistClient.cpp - IIJ Machinist Client for ESP32/ESP8266/SpresenseLTE
    development by nara256  https://github.com/nara256/
    version 0.3

    License MIT
*/

#include <time.h>
#include "IIJMachinistClient.h"
#if defined(ARDUINO_ARCH_ESP32)
  #include "cert_for_esp32.h"
#elif defined(ARDUINO_ARCH_ESP8266)
  #include "cert_for_esp8266.h"
#elif defined(ARDUINO_ARCH_SPRESENSE)
  #include "cert_for_spresense.h"
#endif

#define JST (3600 * 9)
#define NTP_SERVER_HOST "ntp.nict.jp"

#define HEADER_CONTENT_TYPE         "Content-Type"
#define HEADER_CONTENT_TYPE_VALUE   "application/json;charset=UTF-8"
#define HEADER_CONTENT_LENGTH       "Content-Length"
#define HEADER_AUTH                 "Authorization"
#define HEADER_AUTH_PREFIX          "Bearer "

#ifdef ARDUINO_ARCH_SPRESENSE
  LTEUDP ntpUDP;
  NTPClient timeClient(ntpUDP, NTP_SERVER_HOST, JST);
  LTETLSClient tlsClient;
#endif

static String esc(const String &src)
{
    String dest(src);
    dest.replace("\\", "\\\\");
    dest.replace("\"", "\\\"");
    return dest;
}

IIJMachinistClient::IIJMachinistClient(String apiKey)
{
    m_apiKey = apiKey;
}

IIJMachinistClient::IIJMachinistClient(char *apiKey)
{
    m_apiKey = String(apiKey);
}

IIJMachinistClient::~IIJMachinistClient()
{
}

void IIJMachinistClient::debug(const String &val)
{
    if (m_debug != NULL)
    {
        m_debug->println(val);
        yield();
    }
}

void IIJMachinistClient::debug(const char *val)
{
    String s(val);
    debug(s);
}

void IIJMachinistClient::init(void)
{
    setClock();
}

void IIJMachinistClient::setClock(void)
{
#ifdef ARDUINO_ARCH_SPRESENSE
    RTC.begin();
    timeClient.begin();
    timeClient.update();

    RtcTime rtc(timeClient.getEpochTime());
    RTC.setTime(rtc);

    char buff[32];
    sprintf(buff, "%04d/%02d/%02d %02d:%02d:%02d",
         rtc.year(), rtc.month(), rtc.day(),
         rtc.hour(), rtc.minute(), rtc.second());
    debug("Current time: " + String(buff));
#else
    configTime(JST, 0, NTP_SERVER_HOST);

    debug("Waiting for NTP time sync: ");
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        delay(1000);
        debug(".");
        yield();
        nowSecs = time(nullptr);
    }

    time_t t_now = time(NULL);
    debug("Current time: " + String(ctime(&t_now)));
#endif
}

String IIJMachinistClient::getMachinistUri(void)
{
    return m_machinistProtocol + "://" 
            + m_machinistHost 
            + ":" + String(m_machinistPort) 
            + m_machinistPath;
}

void IIJMachinistClient::setDebugSerial(Stream &debug)
{
    m_debug = &debug;
}

String IIJMachinistClient::createMachinistJson(
    const String &agent, const String &nspace, const String &name, float value,
    const String &tagName, const String &tagValue, const long timestamp)
{
    String json;
    json += "{";
    json += "\"agent\":\"" + esc(agent) + "\",";
    json += "\"metrics\":[";
    json += "{";
    json += "\"namespace\":\"" + esc(nspace) + "\",";
    json += "\"name\":\"" + esc(name) + "\",";
    if (!tagName.equals(""))
        json += "\"tags\":{\"" + esc(tagName) + "\":\"" + esc(tagValue) + "\"},";
    json += "\"data_point\":{";
    if (timestamp >= 0)
        json += "\"timestamp\":" + String(timestamp) + ",";
    json += "\"value\":" + String(value);
    json += "}";  // end of data_point
    json += "}]"; // end of metrics
    json += "}";  //end of root
    return json;
}

int IIJMachinistClient::post(
    const String &agent, const String &nspace, const String &name, const float value)
{
    String na("");
    return post(agent, nspace, name, value, na, na);
}

int IIJMachinistClient::post(
    const char *agent, const char *nspace, const char *name, const float value,
    const char *tagName, const char *tagValue, const long timestamp)
{
    String a(agent), ns(nspace), n(name), tn(tagName), tv(tagValue);
    return post(a, ns, n, value, tn, tv, timestamp);
}

int IIJMachinistClient::post(
    const String &agent, const String &nspace, const String &name, const float value,
    const String &tagName, const String &tagValue, const long timestamp)
{
    String json = createMachinistJson(agent, nspace, name, value, tagName, tagValue, timestamp);
    return post(json);
}

int IIJMachinistClient::post(const String &json)
{
    int status = -1;
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
#if defined(ARDUINO_ARCH_ESP32)
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        client->setCACert(ROOT_CERT);
#else
    std::unique_ptr<BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    if (client)
    {
        client->setFingerprint(FINGERPRINT);
#endif
        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient https;
            debug("[HTTPS] begin...  URI=" + getMachinistUri());
            if (https.begin(*client, getMachinistUri()))
            {
                https.addHeader(String(HEADER_CONTENT_TYPE), String(HEADER_CONTENT_TYPE_VALUE));
                https.addHeader(String(HEADER_AUTH), String(HEADER_AUTH_PREFIX) + m_apiKey);
                debug("POST " + json);
                status = https.POST(json);
                debug("status=" + String(status));
                https.end();
            }
            else
            {
                debug("connection failed!");
            }
        }
#ifdef ARDUINO_ARCH_ESP32
        delete client;
#endif
    }
#endif // defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)

#if defined(ARDUINO_ARCH_SPRESENSE)
    tlsClient.setCACert(ROOT_CERT);
    HttpClient client = HttpClient(tlsClient, m_machinistHost, m_machinistPort);
    client.beginRequest();
    client.post(m_machinistPath);
    client.sendHeader(HEADER_CONTENT_TYPE, HEADER_CONTENT_TYPE_VALUE);
    client.sendHeader(HEADER_CONTENT_LENGTH, json.length());
    client.sendHeader(String(HEADER_AUTH), String(HEADER_AUTH_PREFIX) + m_apiKey);
    client.beginBody();
    debug("POST " + json);
    client.print(json);
    client.endRequest();
    status = client.responseStatusCode();
    debug("status=" + String(status));
#endif //defined(ARDUINO_ARCH_SPRESENSE)
    return status;
}

void IIJMachinistClient::debugJson(const String &agent, const String &nspace, const String &name, float value,
                                   const String &tagName, const String &tagValue, const long timestamp)
{
    String json = createMachinistJson(agent, nspace, name, value, tagName, tagValue, timestamp);
    debug(json);
}
