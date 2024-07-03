#include <Arduino.h>
#include "Seeed_Arduino_mmWave.h"

// If the board is an ESP32, include the HardwareSerial library and create a
// HardwareSerial object for the mmWave serial communication
#ifdef ESP32
#  include <HardwareSerial.h>
HardwareSerial mmWaveSerial(0);
#else
// Otherwise, define mmWaveSerial as Serial1
#  define mmWaveSerial Serial1
#endif

SEEED_MR60FDC1 mmWave;

void setup() {
  bool result = false;
  mmWave.begin(&mmWaveSerial);
  Serial.begin(115200);

  float height         = 3;
  float threshold      = 1.0;
  uint32_t sensitivity = 15;
  /**
   * @brief Set the radar installation height, ranging from 1 to 5m.
   */
  result = mmWave.setInstallationHeight(height);  // 3m

  result ? Serial.println("setInstallationHeight success")
         : Serial.println("setInstallationHeight failed");

  /**
   * @brief Set fall threshold 
   * @note The default fall threshold of the radar is 0.6 m.
   */
  result = mmwave.setThreshold(threshold);

  result ? Serial.println("setThreshold success")
         : Serial.println("setThreshold failed");

  result = mmwave.setSensitivity(sensitivity);
  result ? Serial.println("setSensitivity success")
         : Serial.println("setSensitivity failed");
}

void loop() {
  if (mmWave.fetch()) {
    bool is_fall;
    bool is_human;

    mmWave.getHuman(is_human);
    Serial.printf("People Exist: %s\n", is_human ? "true" : "false");

    mmWave.getFall(is_fall);
    Serial.printf("isFall: %s\n", is_fall ? "true" : "false");
  }
}