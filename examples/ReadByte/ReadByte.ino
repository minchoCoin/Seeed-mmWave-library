#include <Arduino.h>

#ifdef ESP32
#include <HardwareSerial.h>
HardwareSerial mmwaveSerial(0);
#else
#define mmwaveSerial Serial1
#endif

void setup() {
    mmwaveSerial.begin(115200);
    Serial.begin(115200);
}

void loop() {
    if(mmwaveSerial.available()) {
        uint8_t data = mmwaveSerial.read();
        Serial.println(data, HEX);
    }
    if(Serial.available()) {
        mmwaveSerial.write(Serial.read());
    }
}