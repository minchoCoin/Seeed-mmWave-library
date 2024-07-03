
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "Seeed_Arduino_mmWave.h"
#ifdef ESP32
#  include <HardwareSerial.h>
HardwareSerial mmwaveSerial(0);
#else
#  define mmwaveSerial Serial1
#endif

const int pixelPin       = D1;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, pixelPin, NEO_GRB + NEO_KHZ800);

void blinkRGB(int r, int g, int b, int times, int delay_ms) {
  for (int i = 0; i < times; ++i) {
    pixels.setPixelColor(0, pixels.Color(r, g, b));
    pixels.show();
    delay(delay_ms);
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));  // 关灯
    pixels.show();
    delay(delay_ms);
  }
}

SEEED_MR60FDC1 mmwave;
void setup() {
  mmwave.begin(&mmwaveSerial);
  delay(1000);
  mmwaveSerial.flush();
  Serial.begin(115200);
  Serial.println("Welcome, my heart is beatin'");

  /* RGB LED */
  pixels.begin();
  pixels.clear();
  pixels.show();
  /* RGB LED */
  pixels.setPixelColor(0, pixels.Color(125, 125, 125));
  bool result;

//   /* set the height of the installation */
//   do {
//     float height = 3;  // 3m
//     result       = mmwave.setInstallationHeight(height);
//     result ? Serial.printf("setInstallationHeight success %.2f m\n", height)
//                  : Serial.println("setInstallationHeight failed");
//     blinkRGB(255, 127, 0, 1, 500);
//   } while (result != true);

//   /* Set threshold */
//   do {
//     float threshold = 1.0;
//     result          = mmwave.setThreshold(threshold);
//     result ? Serial.println("setThreshold success")
//                     : Serial.println("setThreshold failed");
//     if (result) {
//       Serial.printf("threshold: %.2f\n", threshold);
//     }
//     blinkRGB(0, 255, 0, 1, 1000);  //
//   } while (result != true);

//   /* Set sensitivity */
//   do {
//     uint32_t sensitivity = 15;
//     result               = mmwave.setSensitivity(sensitivity);
//     result ? Serial.println("setSensitivity success")
//                          : Serial.println("setSensitivity failed");
//     blinkRGB(255, 127, 0, 1, 500);
//   } while (result != true);

  /* get new parameters of mmwave */
  do {
    float height, threshold, rect_XL, rect_XR, rect_ZF, rect_ZB;

    uint32_t sensitivity;
    result = mmwave.getRadarParameters(height, threshold, sensitivity, rect_XL,
                                       rect_XR, rect_ZF, rect_ZB);
    result ? Serial.println("getRadarParameters3 success")
           : Serial.println("getRadarParameters3 failed");
    if (result) {
      Serial.printf("height: %.2f\tthreshold: %.2f\tsensitivity: %d\n", height,
                    threshold, sensitivity);
      Serial.printf(
          "rect_XL: %.2f\trect_XR: %.2f\trect_ZF: %.2f\trect_ZB: %.2f\n",
          rect_XL, rect_XR, rect_ZF, rect_ZB);
    }
    // delay(3000);
    blinkRGB(255, 127, 0, 1, 500);
    mmwaveSerial.flush();
  } while (result != true);

  // do {
  //     result = mmwave.UserLogInfo(1);
  //     result ? Serial.println("setInstallationHeight
  //     success"):Serial.println("setInstallationHeight failed"); delay(3000);
  // } while (result != true);
}
int cnt = 100000;
// 无人的时候RBG为蓝色
// 有人的时候为绿色
// 跌倒的时候为红色

typedef enum {
  EXIST_PEOPLE,
  NO_PEOPLE,
  PEOPLE_FALL,
} MMWAVE_STATUS;

MMWAVE_STATUS status;

void loop() {
  if (mmwave.fetch()) {
    bool is_fall;
    bool is_human;

    status = NO_PEOPLE;
    mmwave.getHuman(is_human);
    if (is_human) {
      status = EXIST_PEOPLE;
    }
    mmwave.getFall(is_fall);
    if (status == EXIST_PEOPLE && is_fall) {
      status = PEOPLE_FALL;
    }

    switch (status) {
      case NO_PEOPLE:
        Serial.println("Waiting for people");
        pixels.setPixelColor(0, pixels.Color(0, 0, 255));  // BLUE
        break;
      case EXIST_PEOPLE:
        Serial.println("PEOPLE !!!");
        pixels.setPixelColor(0, pixels.Color(0, 255, 0));  // GREEN
        break;
      case PEOPLE_FALL:
        Serial.println("FALL !!!");
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));  // RED
        break;
      default:
        break;
    }
    pixels.show();
    delay(50);
  }
}