/*
    IIJMachinistClient.h - IIJ Machinist Client for ESP32/ESP8266/SpresenseLTE
    development by nara256  https://github.com/nara256/
    version 0.3

    License MIT
*/
#ifndef IIJMachinistClient_h_
#define IIJMachinistClient_h_

#include "Arduino.h"
#if defined(ARDUINO_ARCH_ESP32)
  #include <HTTPClient.h>
  #include <WiFiClientSecure.h>
#elif defined(ARDUINO_ARCH_ESP8266)
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecureBearSSL.h>
#elif defined(ARDUINO_ARCH_SPRESENSE)
  #include <LTEUDP.h>
  #include <LTETLSClient.h>
  #include <RTC.h>
  #include <NTPClient.h>
  #include <ArduinoHttpClient.h>
#endif

#define DEFAULT_MACHINIST_PROTOCOL  "https"
#define DEFAULT_MACHINIST_HOST      "gw.machinist.iij.jp"
#define DEFAULT_MACHINIST_PORT      443
#define DEFAULT_MACHINIST_PATH      "/endpoint"

class IIJMachinistClient
{
private:
    String m_machinistProtocol = String(DEFAULT_MACHINIST_PROTOCOL);
    String m_machinistHost = String(DEFAULT_MACHINIST_HOST);
    int m_machinistPort = DEFAULT_MACHINIST_PORT;
    String m_machinistPath = String(DEFAULT_MACHINIST_PATH);

    String m_apiKey;
    Stream *m_debug = NULL;

    void debug(const String &val);
    void debug(const char *val);

    String createMachinistJson(
        const String &agent, const String &nspace, const String &name, float value,
        const String &tagName, const String &tagValue, const long timestamp);

    void setClock(void);
    String getMachinistUri(void);

public:
    IIJMachinistClient(String apiKey);
    IIJMachinistClient(char *apiKey);
    virtual ~IIJMachinistClient();
    void setDebugSerial(Stream &debug);

    void init(void);
    int post(const String &json);
    int post(const String &agent, const String &nspace, const String &name, const float value);
    int post(const String &agent, const String &nspace, const String &name, const float value,
             const String &tagName, const String &tagValue, const long timestamp = -1L);
    int post(const char *agent, const char *nspace, const char *name, const float value,
             const char *tagName = "", const char *tagValue = "", const long timestamp = -1L);

    void debugJson(const String &agent, const String &nspace, const String &name, const float value,
                   const String &tagName, const String &tagValue, const long timestamp);
};

#endif
