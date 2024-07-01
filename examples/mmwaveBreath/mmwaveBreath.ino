#include <Arduino.h>
#include "Seeed_XIAO_mmWave.h"

#ifdef ESP32
#  include <HardwareSerial.h>
HardwareSerial mmwaveSerial(0);
#else
#  define mmwaveSerial Serial1
#endif

mmWaveBreath mmwave;
void setup() {
  mmwave.begin(&mmwaveSerial);
  Serial.begin(115200);
  Serial.println("Welcome, my heart is beatin'");
}

void loop() {
  if (mmwave.fetch()) {
    float total_phase, breath_phase, heart_phase;
    if (mmwave.getHeartBreathPhases(total_phase, breath_phase, heart_phase)) {
      Serial.printf("total_phase: %.2f\t", total_phase);
      Serial.printf("breath_phase: %.2f\t", breath_phase);
      Serial.printf("heart_phase: %.2f\n", heart_phase);
    }

    float breath_rate;
    if (mmwave.getBreathRate(breath_rate)) {
      Serial.printf("breath_rate: %.2f\n", breath_rate);
    }

    float heart_rate;
    if (mmwave.getHeartRate(heart_rate)) {
      Serial.printf("heart_rate: %.2f\n", heart_rate);
    }

    float distance;
    if (mmwave.getHeartBreathDistance(distance)) {
      Serial.printf("distance: %.2f\n", distance);
    }
  }
}