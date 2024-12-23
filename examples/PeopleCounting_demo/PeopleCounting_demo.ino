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

SEEED_MR60BHA2 mmWave;

void setup() {
  Serial.begin(115200);
  mmWave.begin(&mmWaveSerial);
}

void loop() {
  if (mmWave.update(100)) {
    PeopleCounting point_cloud;
    if (mmWave.getPeopleCountingPointCloud(point_cloud)) {
        Serial.printf("-----Got a Point Cloud-----\n");
        Serial.printf("target_num: %d\n", point_cloud.target_num);
        Serial.printf("x_point: %.2f\n", point_cloud.x_point);
        Serial.printf("y_point: %.2f\n", point_cloud.y_point);
        Serial.printf("dop_index: %d\n", point_cloud.dop_index);
        Serial.printf("cluster_index: %d\n", point_cloud.cluster_index);
    }

    PeopleCounting target_info;
    if (mmWave.getPeopleCountingTartgetInfo(target_info)) {
        Serial.printf("-----Got a Tartget Info-----\n");
        Serial.printf("target_num: %d\n", target_info.target_num);
        Serial.printf("x_point: %.2f\n", target_info.x_point);
        Serial.printf("y_point: %.2f\n", target_info.y_point);
        Serial.printf("dop_index: %d\n", target_info.dop_index);
        Serial.printf("cluster_index: %d\n", target_info.cluster_index);
    }
  }
}