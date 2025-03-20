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
uint32_t sensitivity = 3;
float height = 2.8, threshold = 1.0;
float rect_XL, rect_XR, rect_ZF, rect_ZB;
void setup() {
  Serial.begin(115200);
  mmWave.begin(&mmWaveSerial);
  mmWave.setUserLog(1);
  
 

  /** get new parameters of mmwave **/
  if (mmWave.getRadarParameters(height, threshold, sensitivity, rect_XL,
                                rect_XR, rect_ZF, rect_ZB)) {
    Serial.printf("height: %.2f\tthreshold: %.2f\tsensitivity: %d\n", height,
                  threshold, sensitivity);
    Serial.printf(
        "rect_XL: %.2f\trect_XR: %.2f\trect_ZF: %.2f\trect_ZB: %.2f\n", rect_XL,
        rect_XR, rect_ZF, rect_ZB);
  } else {
    Serial.println("getRadarParameters failed");
  }
}

void loop() {
  if (mmWave.update(100)) {
    
    PeopleCounting target_info;
    if (mmWave.getPeopleCountingPointCloud(target_info)) {
      if (target_info.targets.size()>0){
        Serial.print("{\"targets\": [");
        for (size_t i = 0; i < target_info.targets.size(); i++) {
          const auto& target = target_info.targets[i];
    
          Serial.print("{");
          Serial.printf("\"target_id\": %zu,", i + 1);
          Serial.printf("\"x_point\": %.2f,", target.x_point);
          Serial.printf("\"y_point\": %.2f,", target.y_point);
          Serial.printf("\"z_point\": %.2f,", target.z_point);
          Serial.printf("\"dop_index\": %f,", target.dop_index);
          Serial.printf("\"cluster_index\": %f,", target.cluster_index);
          Serial.printf("\"move_speed\": %.2f", target.dop_index * 100);
          Serial.print("}");
    
          // Add comma if not the last item
          if (i < target_info.targets.size() - 1) {
          Serial.print(",");
          } 
        }
        Serial.println("]}");
      }
    }

  // delay(500);
  }
}