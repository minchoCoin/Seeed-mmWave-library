#include <Arduino.h>
#include "mmWaveBreath.h"

#ifdef ESP32
#include <HardwareSerial.h>
HardwareSerial mmwaveSerial(0);
#else
#define mmwaveSerial Serial1
#endif

mmWaveBreath mmwave(&mmwaveSerial);
void setup() {
    mmwave.begin();
    Serial.begin(115200);
    Serial.println("Welcome, my heart is beatin'");

}

void loop(){
    if(mmwave.fetch() > 0){
        mmwave.print_data();
    }
}