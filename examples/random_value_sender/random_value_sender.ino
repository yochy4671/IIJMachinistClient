#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include "IIJMachinistClient.h"

#define WIFI_ID "YOUR WIFI SSID HERE"
#define WIFI_PASS "YOUR WIFI PASSWORD HERE"
#define MACHINIST_API_KEY "YOUR Machinist API KEY here"

IIJMachinistClient c(MACHINIST_API_KEY);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(3000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_ID, WIFI_PASS);

  // wait for WiFi connection
  Serial.print("\nWaiting for WiFi to connect...");
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" connected");

  randomSeed(analogRead(0));
  c.setDebugSerial(Serial);
  c.init();
}

void loop()
{
  float value = (float)random(10000) / 100.;

  //あらかじめ Machinist の UI から "test" という名称の Agent を作成しておきます。
  int s = c.post("test", "NAME\\SPACE", "NAME", value, "KEY", "VALUE");
  if (s == 200)
    Serial.println("OK");
  else
    Serial.println("NG status=" + String(s));
  delay(60 * 1000);
}
