#include <WiFi.h>
#include <WiFiMulti.h>
#include "IIJMachinistClient.h"

#define WIFI_ID   "YOUR WIFI SSID HERE"
#define WIFI_PASS "YOUR WIFI PASSWORD HERE"
#define MACHINIST_API_KEY "YOUR Machinist API KEY here"

WiFiMulti wiFiMulti;  
IIJMachinistClient c(MACHINIST_API_KEY);

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  wiFiMulti.addAP(WIFI_ID, WIFI_PASS);

  // wait for WiFi connection
  Serial.print("Waiting for WiFi to connect...");
  while ((wiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  randomSeed(analogRead(0));
  c.setDebugSerial(Serial);
  c.init();
}

void loop() {
  float value = (float)random(10000)/100.;

  //あらかじめ Machinist の UI から "test" という名称の Agent を作成しておきます。
  int s = c.post("test", "NAME\\SPACE", "NAME", value, "KEY", "VALUE");
  if(s==200) {
    Serial.println("OK");
  } else
    Serial.println("NG status=" + String(s));
  }
  delay(60*1000);
}
