#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// If the board is an ESP32, include the HardwareSerial library and create a HardwareSerial object for the mmWave serial communication
#ifdef ESP32
#include <HardwareSerial.h>
HardwareSerial mmwaveSerial(0);
#else
// Otherwise, define mmwaveSerial as Serial1
#define mmwaveSerial Serial1
#endif

// Define the pin for the NeoPixel
const int pixelPin       = D1;

// Create an instance of the Adafruit_NeoPixel class, specifying the number of pixels, the pin, and the pixel type (NEO_GRB + NEO_KHZ800)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(1, pixelPin, NEO_GRB + NEO_KHZ800);

void setup() {
    // Initialize the serial communication at 115200 baud
    Serial.begin(115200);
    mmwaveSerial.begin(115200);

    // Initialize the NeoPixel
    pixels.begin();
    pixels.clear();
    pixels.show();
}

void loop() {
    // Blink the NeoPixel 10 times
    for (int i = 0; i < 10; i++) {
        // Set the pixel color to red
        pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        pixels.show();
        delay(100);
        // Set the pixel color to black
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        pixels.show();
        delay(100);
    }
    // Fade the NeoPixel from red to black
    for (int i = 255; i >= 0; i--) {
        pixels.setPixelColor(0, pixels.Color(i, 0, 0));
        pixels.show();
        delay(10);
    }
}