/*
    IIJMachinistClient.cpp - IIJ Machinist Client for ESP32
    development by nara256  https://github.com/nara256/
    version 0.1

    License MIT
*/

#include "IIJMachinistClient.h"
#include "cert.h"

#define JST (3600 * 9)

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
    configTime(JST, 0,
               "ntp.nict.jp",
               "ntp.jst.mfeed.ad.jp");

    debug("Waiting for NTP time sync: ");
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        delay(1000);
        debug(".");
        yield();
        nowSecs = time(nullptr);
    }

    struct tm timeInfo;
    getLocalTime(&timeInfo);
    debug("Current time: " + String(asctime(&timeInfo)));
}

String IIJMachinistClient::getMachinistUri(void)
{
    return m_machinistUri;
}

void IIJMachinistClient::setMachinistUri(char *uri)
{
    String u(uri);
    setMachinistUri(u);
}
void IIJMachinistClient::setMachinistUri(String &uri)
{
    m_machinistUri = uri;
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
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        client->setCACert(ROOT_CERT);
        {
            // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
            HTTPClient https;
            debug("[HTTPS] begin...  URI=" + m_machinistUri);
            if (https.begin(*client, m_machinistUri))
            {
                https.addHeader(String("Content-type"), String("application/json;charset=UTF-8"));
                https.addHeader(String("Authorization"), String("Bearer ") + m_apiKey);
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
        delete client;
    }
    return status;
}

void IIJMachinistClient::debugJson(const String &agent, const String &nspace, const String &name, float value,
                                   const String &tagName, const String &tagValue, const long timestamp)
{
    String json = createMachinistJson(agent, nspace, name, value, tagName, tagValue, timestamp);
    debug(json);
}
