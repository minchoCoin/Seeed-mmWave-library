# Seeed Arduino mmWave Library For XIAO

## Introduction

This Library is designed to interface with the Seeed Studio XIAO ESP32-C6 Board integrated on the [MR60BHA2-XIAO 60GHz mmWave Human Breathing and Heartbeat Sensor](https://www.seeedstudio.com/MR60BHA2-60GHz-mmWave-Sensor-Breathing-and-Heartbeat-Module-p-5945.html) and [MR60FDA2-XIAO 60GHz mmWave Human Fall Detection Sensor](https://www.seeedstudio.com/MR60FDA2-60GHz-mmWave-Sensor-Fall-Detection-Module-p-5946.html)  sensors. This library enables easy data reading and additional operations for functionalities such as breathing and heartbeat monitoring, and fall detection.

### Supported Seeed Studio Devices

| Device                           | Functionality                  |
| -------------------------------- | ------------------------------ |
| [**MR60BHA2** 60GHz mmWave Sensor](https://www.seeedstudio.com/MR60BHA2-60GHz-mmWave-Sensor-Breathing-and-Heartbeat-Module-p-5945.html) | Breathing and Heartbeat Module |
| [**MR60FDA2** 60GHz mmWave Sensor](https://www.seeedstudio.com/MR60FDA2-60GHz-mmWave-Sensor-Fall-Detection-Module-p-5946.html) | Fall Detection Module          |

## Installation

1. Download the library from GitHub or the Arduino Library Manager.
2. Open the Arduino IDE.
3. Go to `Sketch` > `Include Library` > `Add .ZIP Library...` and select the downloaded file.

## Usage

To begin using the library, include it in your Arduino sketch and refer to the examples folder.

```cpp
#include "Seeed_Arduino_mmWave.h"
```

If you'd like to watch the debug info, Set _MMWAVE_DEBUG to 1 in SeeedmmWave.h

```cpp
#define _MMWAVE_DEBUG 1
// src/SeeedmmWave.h
```

### Examples

- **GroveU8x8:** Demonstrates how to utilize Grove GPIO pins to interface with the Grove - OLED Display 0.96" using the U8x8 library. This example shows basic text display functions.

- **LightRGB:** Provides an example of controlling an RGB LED.

- **mmWaveBreath:** Demonstrates how to use the MR60BHA2 sensor for monitoring breathing and heartbeat. It covers initializing the sensor, fetching data, and displaying breathing rate, heart rate, and phase information.

- **mmWaveFall:** Illustrates how to use the MR60FDA2 sensor for fall detection. This example includes initializing the sensor and processing fall detection data to trigger alerts.

- **ReadByte:** A basic example that reads bytes of data from a sensor or serial input and displays the raw data. Useful for debugging and understanding data formats.

- **ReadLuxValue:** Shows how to read lux (light intensity) values from the onboard light sensor. It demonstrates sensor initialization, reading, and printing lux values.

- **PeopleCounting:** Illustrates how to use the MR60BHA2 sensor for counting people

- **PointCloud:** Illustrates how to use the MR60FDA2 sensor for getting point cloud

- **gui_firmware:** ESP32C6 firmware for using [GUI Software](https://wiki.seeedstudio.com/getting_started_with_mr60fda2_mmwave_kit/#resources)

### PointCloud output example
```
16:55:06.157 -> ESP-ROM:esp32c6-20220919
16:55:06.390 -> height: 2.80	threshold: 0.50	sensitivity: 3
16:55:06.390 -> rect_XL: 0.30	rect_XR: 0.30	rect_ZF: 0.30	rect_ZB: 0.30
16:55:08.515 -> Number of targets: 3
16:55:08.515 -> Target 1:
16:55:08.515 ->   x_point: 0.14
16:55:08.515 ->   y_point: 0.51
16:55:08.515 ->   dop_index: 0.033011
16:55:08.515 ->   cluster_index: 0.000000
16:55:08.515 ->   move_speed: 0.57 cm/s
16:55:08.515 -> Target 2:
16:55:08.515 ->   x_point: 0.00
16:55:08.515 ->   y_point: 0.13
16:55:08.515 ->   dop_index: 0.588328
16:55:08.515 ->   cluster_index: 0.000000
16:55:08.515 ->   move_speed: 10.17 cm/s
16:55:08.515 -> Target 3:
16:55:08.515 ->   x_point: 0.01
16:55:08.515 ->   y_point: 0.00
16:55:08.515 ->   dop_index: 0.101009
16:55:08.515 ->   cluster_index: 0.000000
16:55:08.515 ->   move_speed: 1.75 cm/s
...
```

## API Reference

Please refer to the comments in the source code for detailed information on the available methods and parameters.

## License

This library is released under the [MIT License](https://github.com/love4yzp/Seeed-mmWave-library/blob/main/LICENSE).

## Contributing
Contributions to the library are welcome. Please follow the standard pull request process to suggest improvements or add new features.

# References
- [Getting started with 60GHz mmWave Fall Detection Sensor Kit with XIAO ESP32C6 (MR60FDA2)](https://wiki.seeedstudio.com/getting_started_with_mr60fda2_mmwave_kit/)

- [Getting started with 60GHz mmWave Breathing and Heartbeat Detection Sensor Kit with XIAO ESP32C6 (MR60BHA2)](https://wiki.seeedstudio.com/getting_started_with_mr60bha2_mmwave_kit/)