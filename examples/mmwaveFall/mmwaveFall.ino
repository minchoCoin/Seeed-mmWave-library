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

SEEED_MR60FDA2 mmWave;

void setup() {
  bool result = false;
  Serial.begin(115200);
  while (!Serial)
    continue;
  mmWave.begin(&mmWaveSerial);
  delay(3000);
  float height         = 2.2;
  float threshold      = 1.0;
  uint32_t sensitivity = 15;

  result = mmWave.setUserLog(0);

  result ? Serial.println("setUserLog success")
         : Serial.println("setUserLog failed");

  /**
   * @brief Set the radar installation height, ranging from 1 to 5m.
   */
  do {
    result = mmWave.setInstallationHeight(height);  // 3m

    result ? Serial.println("setInstallationHeight success")
           : Serial.println("setInstallationHeight failed");
    delay(3000);
  } while (!result);

  /**
   * @brief Set fall threshold
   * @note The default fall threshold of the radar is 0.6 m.
   */
  result = mmWave.setThreshold(threshold);

  result ? Serial.println("setThreshold success")
         : Serial.println("setThreshold failed");

  result = mmWave.setSensitivity(sensitivity);
  result ? Serial.println("setSensitivity success")
         : Serial.println("setSensitivity failed");

  float rect_XL, rect_XR, rect_ZF, rect_ZB;
  result = mmWave.getRadarParameters(height, threshold, sensitivity, rect_XL,
                                     rect_XR, rect_ZF, rect_ZB);
  result ? Serial.println("getRadarParameters success")
         : Serial.println("getRadarParameters failed");
  if (result) {
    Serial.printf("height: %.2f\tthreshold: %.2f\tsensitivity: %d\n", height,
                  threshold, sensitivity);
    Serial.printf(
        "rect_XL: %.2f\trect_XR: %.2f\trect_ZF: %.2f\trect_ZB: %.2f\n", rect_XL,
        rect_XR, rect_ZF, rect_ZB);
  }
}

void loop() {
  if (mmWave.update(100)) {
    bool is_fall;
    bool is_human;
    is_human = mmWave.getHuman();
    Serial.printf("People Exist: %s\n", is_human ? "true" : "false");

    is_fall = mmWave.getFall();
    Serial.printf("isFall: %s\n", is_fall ? "true" : "false");
  }
  delay(10);
}