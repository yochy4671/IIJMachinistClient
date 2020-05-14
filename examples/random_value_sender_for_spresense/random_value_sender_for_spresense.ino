#include <LTE.h>
#include "IIJMachinistClient.h"

#define LTE_APN       "apn"      // replace your APN
#define LTE_USER_NAME "user"     // replace with your username
#define LTE_PASSWORD  "password" // replace with your password

#define MACHINIST_API_KEY "YOUR Machinist API KEY here"

LTE lteAccess;
IIJMachinistClient c(MACHINIST_API_KEY);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(3000);

  while (true) {
    if (lteAccess.begin() == LTE_SEARCHING) {
      if (lteAccess.attach(LTE_APN, LTE_USER_NAME, LTE_PASSWORD) == LTE_READY) {
        Serial.println("attach succeeded.");
        break;
      }
      Serial.println("An error occurred, shutdown and try again.");
      lteAccess.shutdown();
      sleep(1);
    }
  }

  randomSeed(analogRead(A0));
  c.setDebugSerial(Serial);
  c.init();
}

void loop()
{
  float value = (float)random(10000) / 100.;

  //あらかじめ Machinist の UI から "test" という名称の Agent を作成しておきます。
  int s = c.post("test", "NAMESPACE", "NAME", value, "KEY", "VALUE");
  if (s == 200)
    Serial.println("OK");
  else
    Serial.println("NG status=" + String(s));
  delay(60 * 1000);
}
