/*
    IIJMachinistClient.h - IIJ Machinist Client for ESP32/ESP8266
    development by nara256  https://github.com/nara256/
    version 0.2

    License MIT
*/
#ifndef IIJMachinistClient_h_
#define IIJMachinistClient_h_

#include "Arduino.h"
#ifdef ARDUINO_ARCH_ESP32
  #include <HTTPClient.h>
  #include <WiFiClientSecure.h>
#else
  #include <ESP8266HTTPClient.h>
  #include <WiFiClientSecureBearSSL.h>
#endif


#define DEFAULT_MACHINIST_URI "https://gw.machinist.iij.jp/endpoint"

class IIJMachinistClient
{
private:
    String m_machinistUri = String(DEFAULT_MACHINIST_URI);
    String m_apiKey;
    Stream *m_debug = NULL;

    void debug(const String &val);
    void debug(const char *val);

    String createMachinistJson(
        const String &agent, const String &nspace, const String &name, float value,
        const String &tagName, const String &tagValue, const long timestamp);

    void setClock(void);

public:
    IIJMachinistClient(String apiKey);
    IIJMachinistClient(char *apiKey);
    virtual ~IIJMachinistClient();
    String getMachinistUri(void);
    void setMachinistUri(String &uri);
    void setMachinistUri(char *uri);
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
