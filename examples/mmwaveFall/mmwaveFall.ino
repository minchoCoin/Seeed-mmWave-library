#include <Arduino.h>
#include "Seeed_XIAO_mmWave.h"

#ifdef ESP32
#  include <HardwareSerial.h>
HardwareSerial mmwaveSerial(0);
#else
#  define mmwaveSerial Serial1
#endif

mmWaveFall mmwave;
void setup() {
  mmwave.begin(&mmwaveSerial);
  Serial.begin(115200);
  Serial.println("Do not Fall, I will take care of you");
  bool result = false;
  do {
    result = mmwave.setInstallationHeight(3);  // 3m
    result ? Serial.println("setInstallationHeight success")
           : Serial.println("setInstallationHeight failed");
    delay(3000);
  } while (result != true);
}

void loop() {
  if (mmwave.fetch()) {
    bool is_fall;
    bool is_human;

    mmwave.getHuman(is_human);
    Serial.printf("People Exist: %s\n", is_human ? "true" : "false");

    mmwave.getFall(is_fall);
    Serial.printf("isFall: %s\n", is_fall ? "true" : "false");
  }
}