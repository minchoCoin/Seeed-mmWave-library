#include "mmWave.h"
#include <Arduino.h>

#ifdef ESP32
#include <HardwareSerial.h>
HardwareSerial mmwaveSerial(0);
#else
#define mmwaveSerial Serial1
#endif

mmWaveFall mmwave;
void setup() {
    mmwave.begin(&mmwaveSerial);
    Serial.begin(115200);
    Serial.println("Do not Fall, I will take care of you");
    bool result = false;
    do {
        result = mmwave.setInstallationHeight(3);  // 3m
        result ? Serial.println("setInstallationHeight success") : Serial.println("setInstallationHeight failed");
        delay(3000);
    } while (result != true);
}

void loop() {
    if (mmwave.fetch(1000)) {
        if (mmwave.isFallUpdated()) {
            bool is_fall = mmwave.isFall();
            Serial.printf("isFall: %s\n", is_fall ? "true" : "false");
        }
    }
}