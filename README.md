# Seeed Arduino mmWave Library For XIAO

## Introduction

This Library is designed to interface with the Seeed Studio XIAO Board on the MR60BHA2 and MR60FDA2 60GHz mmWave sensors. This library enables easy data reading and additional operations for functionalities such as breathing and heartbeat monitoring, and fall detection.

## Features

- **Breathing and Heartbeat Monitoring (MR60BHA2)**
- **Fall Detection (MR60FDA2)**

## Installation

1. Download the library from GitHub or the Arduin Library Manager.
2. Open the Arduino IDE.
3. Go to `Sketch` > `Include Library` > `Add .ZIP Library...` and select the downloaded file.

## Usage

To start using the library, include it in your Arduino sketch and refer to the examples folder.

```cpp
#include <Arduino.h>
#include "Seeed_Arduino_mmWave.h"

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
```

### Examples

- **GroveU8X8:** Demonstrates how to utilize Grove GPIO Pins to interface with the Grove - OLED Display 0.96" using the U8X8 library. This example shows basic text display functions.

- **LightRGB:** Provides an example of controlling an RGB LED

- **mmWaveBreath:** Demonstrates the usage of the MR60BHA2 sensor for monitoring breathing and heartbeat. It shows how to initialize the sensor, fetch data, and display breathing rate, heart rate, and phase information.

- **mmWaveFall:** Illustrates how to use the MR60FDA2 sensor for fall detection. This example includes initializing the sensor and processing the fall detection data to trigger alerts.

- **ReadByte:** A basic example to read bytes of data from a sensor or serial input and display the raw data. Useful for debugging and understanding the data format.

- **ReadLuxValue:** Shows how to read lux (light intensity) values from the onboard light sensor. It demonstrates the initialization of the sensor and reading and printing the lux values.

## API Reference

Please refer to the comments in the source code for detailed information on the available methods and parameters.

## License

This library is released under the [MIT License](https://github.com/love4yzp/Seeed-mmWave-library/blob/main/LICENSE).

## Contributing
Contributions to the library are welcome. Please follow the standard pull request process to suggest improvements or add new features.