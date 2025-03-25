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
17:41:02.478 -> ESP-ROM:esp32c6-20220919
17:41:02.478 -> height: 2.80	threshold: 0.50	sensitivity: 3
17:41:02.478 -> rect_XL: 0.30	rect_XR: 0.30	rect_ZF: 0.30	rect_ZB: 0.30
17:41:04.311 -> Number of targets: 5
17:41:04.311 -> Target 1:
17:41:04.311 ->   x_point: 0.17
17:41:04.311 ->   y_point: 0.50
17:41:04.311 ->   z_point: 0.00
17:41:04.311 ->   dop_index: 0.018602
17:41:04.311 ->   cluster_index: 0.000000
17:41:04.311 ->   move_speed: 0.32 cm/s
17:41:04.311 -> Target 2:
17:41:04.353 ->   x_point: -0.21
17:41:04.353 ->   y_point: 0.35
17:41:04.353 ->   z_point: 0.20
17:41:04.353 ->   dop_index: 0.037204
17:41:04.353 ->   cluster_index: 0.000000
17:41:04.353 ->   move_speed: 0.64 cm/s
17:41:04.353 -> Target 3:
17:41:04.353 ->   x_point: -0.06
17:41:04.353 ->   y_point: 0.46
17:41:04.353 ->   z_point: 0.25
17:41:04.353 ->   dop_index: 0.037204
17:41:04.353 ->   cluster_index: 0.000000
17:41:04.353 ->   move_speed: 0.64 cm/s
17:41:04.353 -> Target 4:
17:41:04.353 ->   x_point: -0.27
17:41:04.353 ->   y_point: 0.30
17:41:04.353 ->   z_point: 0.20
17:41:04.353 ->   dop_index: 0.046505
17:41:04.353 ->   cluster_index: 0.000000
17:41:04.353 ->   move_speed: 0.80 cm/s
17:41:04.353 -> Target 5:
17:41:04.353 ->   x_point: -0.29
17:41:04.353 ->   y_point: 0.44
17:41:04.353 ->   z_point: -0.00
17:41:04.353 ->   dop_index: 0.046505
17:41:04.353 ->   cluster_index: 0.000000
17:41:04.353 ->   move_speed: 0.80 cm/s
...
```

### How to get PointCloudChart
1. Flash PointCloudChart_demo\PointCloudChart_demo_ino\PointCloudChart_demo_ino.ino to ESP32C6
2. Python requirements: pyserial, numpy, matplotlib, mpl_toolkits
3. run python code: PointCloudChart_demoPointCloudChart_realtime.py (for realtime and save fig) or PointCloudChart_demoPointCloudChart.py (for only save fig)

### PointCloudChart Demo video and sample
- [https://youtu.be/nghvNvRgPC0](https://youtu.be/nghvNvRgPC0)
- [https://youtube.com/shorts/jPIVUtv3SSc?feature=share](https://youtube.com/shorts/jPIVUtv3SSc?feature=share)

Please see PointCloudChart_demo_ino.ino, PointCloudChart_realtime.py and PointCloud2dChart_realtime.py


![image](https://github.com/minchoCoin/Seeed-mmWave-library/blob/main/PointCloudChart_demo/PointCloudChartPNG/3D%20Point%20Cloud%20-%208%20(7043ms).png)
![image](https://github.com/minchoCoin/Seeed-mmWave-library/blob/main/PointCloudChart_demo/PointCloud2dChartPNG/2D_Projections_10_81834ms.png)
## API Reference

Please refer to the comments in the source code for detailed information on the available methods and parameters.

## License

This library is released under the [MIT License](https://github.com/love4yzp/Seeed-mmWave-library/blob/main/LICENSE).

## Contributing
Contributions to the library are welcome. Please follow the standard pull request process to suggest improvements or add new features.

# References
- [Getting started with 60GHz mmWave Fall Detection Sensor Kit with XIAO ESP32C6 (MR60FDA2)](https://wiki.seeedstudio.com/getting_started_with_mr60fda2_mmwave_kit/)

- [Getting started with 60GHz mmWave Breathing and Heartbeat Detection Sensor Kit with XIAO ESP32C6 (MR60BHA2)](https://wiki.seeedstudio.com/getting_started_with_mr60bha2_mmwave_kit/)

- [https://github.com/Love4yzp/Seeed-mmWave-library](https://github.com/Love4yzp/Seeed-mmWave-library)

- [MR60BHA2 Tiny Frame Interface Manual](https://wiki.seeedstudio.com/getting_started_with_mr60bha2_mmwave_kit/#resources)