/**
 * @file mmWave.h
 * @date  12 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef SEEED_XIAO_MMWAVE_H
#define SEEED_XIAO_MMWAVE_H

#define _VERSION_MMWAVEBREATH_0_0_1 "0.0.9"

#include <Arduino.h>
#include <memory>
#include <queue>
#include "mmWaveBreathTypes.hpp"

#ifndef _UART_BAUD
#  define _UART_BAUD 115200
#endif

#define MAX_QUEUE_SIZE    10
#define FRAME_BUFFER_SIZE 512
#define SOF_BYTE          0x01

// Frame structure sizes
#define SIZE_SOF        1
#define SIZE_ID         2
#define SIZE_LEN        2
#define SIZE_TYPE       2
#define SIZE_HEAD_CKSUM 1
#define SIZE_FRAME_HEADER                                                      \
  (SIZE_SOF + SIZE_ID + SIZE_LEN + SIZE_TYPE + SIZE_HEAD_CKSUM)
#define SIZE_DATA_CKSUM 1

typedef enum SeeedmmWaveError {
  mmWaveInvlid,
  mmWaveOK,
  mmWaveError,
} SeeedmmWaveError;

class SeeedmmWave {
 private:
  HardwareSerial* _serial;
  uint32_t _baud;
  uint32_t _wait_delay;

 protected:
  uint8_t calculateChecksum(const uint8_t* data, size_t len);
  bool validateChecksum(const uint8_t* data, size_t len,
                        uint8_t expected_checksum);
  bool processFrame(const uint8_t* frame_bytes, size_t len, uint16_t data_type);

  virtual bool handleType(uint16_t _type, const uint8_t* data,
                          size_t data_len) = 0;

 public:
  SeeedmmWave();
  ~SeeedmmWave();
  void begin(HardwareSerial* serial, uint32_t baud = _UART_BAUD,
             uint32_t wait_delay = 1, int rst = -1);
  int available();
  int read(char* data, int length);
  int write(const char* data, int length);
  bool SendFrame(const uint16_t type, const uint8_t* data, size_t data_len);

  bool fetch(uint16_t data_type = 0xFFFF, uint32_t timeout = 1);

  float extractFloat(const uint8_t* bytes) const;
  uint32_t extractU32(const uint8_t* bytes) const;
  void floatToBytes(float value, uint8_t* bytes);
  void uint32ToBytes(uint32_t value, uint8_t* bytes);
};

class mmWaveBreath : public SeeedmmWave {
 private:
  HeartBreath* _heartBreath;  // Raw pointer for HeartBreath
  BreathRate* _breathRate;    // Raw pointer for BreathRate
  HeartRate* _heartRate;      // Raw pointer for HeartRate
  HeartBreathDistance*
      _heartBreathDistance;  // Raw pointer for HeartBreathDistance

 public:
  mmWaveBreath()
      : _heartBreath(nullptr),
        _breathRate(nullptr),
        _heartRate(nullptr),
        _heartBreathDistance(nullptr) {}

  virtual ~mmWaveBreath() {
    delete _heartBreath;
    delete _breathRate;
    delete _heartRate;
    delete _heartBreathDistance;
  }

  bool handleType(uint16_t _type, const uint8_t* data,
                  size_t data_len) override {
    // First, delete any existing objects to avoid memory leaks
    delete _heartBreath;
    _heartBreath = nullptr;
    delete _breathRate;
    _breathRate = nullptr;
    delete _heartRate;
    _heartRate = nullptr;
    delete _heartBreathDistance;
    _heartBreathDistance = nullptr;

    TypeHeartBreath type = static_cast<TypeHeartBreath>(_type);
    switch (type) {
      case TypeHeartBreath::TypeHeartBreathPhase:
        _heartBreath = new HeartBreath(extractFloat(data),
                                       extractFloat(data + sizeof(float)),
                                       extractFloat(data + 2 * sizeof(float)));
        break;
      case TypeHeartBreath::TypeBreathRate:
        _breathRate = new BreathRate(extractFloat(data));
        break;
      case TypeHeartBreath::TypeHeartRate:
        _heartRate = new HeartRate(extractFloat(data));
        break;
      case TypeHeartBreath::TypeHeartBreathDistance:
        _heartBreathDistance = new HeartBreathDistance(
            extractU32(data), extractFloat(data + sizeof(uint32_t)));
        break;
      default:
        return false;  // Unhandled type
    }
    return true;
  }

  bool getHeartBreathPhases(float& total_phase, float& breath_phase,
                            float& heart_phase) const {
    if (_heartBreath && _heartBreath->isValid()) {
      _heartBreath->getPhase(total_phase, breath_phase, heart_phase);
      return true;
    }
    return false;
  }

  bool getBreathRate(float& rate) const {
    if (_breathRate && _breathRate->isValid()) {
      _breathRate->getBreathRate(rate);
      return true;
    }
    return false;
  }

  bool getHeartRate(float& rate) const {
    if (_heartRate && _heartRate->isValid()) {
      _heartRate->getHeartRate(rate);
      return true;
    }
    return false;
  }

  bool getHeartBreathDistance(float& distance) const {
    if (_heartBreathDistance && _heartBreathDistance->isValid()) {
      _heartBreathDistance->getDistance(distance);
      return true;
    }
    return false;
  }
};

class mmWaveFall : public SeeedmmWave {
 private:
  // get fall detection
  bool _isFall        = false;
  bool _isFallUpdated = false;

  // set height
  bool _HightValid = false;

  //  get parameters
  bool _parametersValid = false;
  float _height;
  float _thershold;
  uint32_t _sensitivity;
  float _rect_XL;
  float _rect_XR;
  float _rect_ZF;
  float _rect_ZB;

  float set_threshold;  // deault value=0.6m TODO 这个是干什么用的？
  bool is_threshold_valid = false;

  bool _isAlarmAreaValid;

  bool _isSensitivityValid;
  // bool isSensitivityValid() const { return sensitivity > 0; }
  bool _isHuman;

 public:
  mmWaveFall() { /*需要开始就把参数信息发送过来*/ }
  virtual ~mmWaveFall() {}
  bool handleType(uint16_t _type, const uint8_t* data,
                  size_t data_len) override {
    TypeFallDetection type = static_cast<TypeFallDetection>(_type);
    switch (type) {
      case TypeFallDetection::ReportFallDetection:
        _isFallUpdated = true;
        _isFall        = *(const bool*)data;
        break;
      case TypeFallDetection::InstallationHeight: {
        if (data_len != 1)
          return false;
        _HightValid = *(const uint8_t*)data;
        break;
      }
      case TypeFallDetection::RadarParameters: {
        if (data_len <= 1)
          return false;
        _height          = extractFloat(data);
        _thershold       = extractFloat(data + sizeof(float));
        _sensitivity     = extractU32(data + 2 * sizeof(float));
        _rect_XL         = extractFloat(data + 3 * sizeof(float));
        _rect_XR         = extractFloat(data + 4 * sizeof(float));
        _rect_ZF         = extractFloat(data + 5 * sizeof(float));
        _rect_ZB         = extractFloat(data + 6 * sizeof(float));
        _parametersValid = true;
        break;
      }
      case TypeFallDetection::FallThreshold: {  // set fall threshold result
        if (data_len != 1)
          return false;
        is_threshold_valid = *(const uint8_t*)data;
        break;
      }
      case TypeFallDetection::AlarmParameters: {
        _isAlarmAreaValid = *(const uint8_t*)data;
        break;
      }
      case TypeFallDetection::FallSensitivity:
        _isSensitivityValid = *(const uint8_t*)data;
        break;
      case TypeFallDetection::ReportUnmannedDetection:
        _isHuman = *(const uint8_t*)data;
        break;
      default:
        return false;
    }
    return true;
  }

  /**
   * @brief Set the height of the installation
   * @param height The height of the installation in meters(m)
   * @note Height range: 1.0m - 5.0m
   * @return True if the installation height was set successfully
   */
  bool setInstallationHeight(float height) {
    uint8_t data[sizeof(float)];
    floatToBytes(height, data);
    uint16_t type =
        static_cast<uint16_t>(TypeFallDetection::InstallationHeight);
    if (this->SendFrame(type, data, sizeof(data))) {
      return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
    }
    return false;
  }

  bool getFall(bool& fall_status) {
    if (_isFallUpdated) {
      fall_status    = _isFall;
      _isFallUpdated = false;
      return true;
    }
    return false;
  }

  /**
   * @brief Obtain the Radar parameters
   */
  bool getRadarParameters(float& height, float& threshold,
                          uint32_t& sensitivity) {
    getRadarParameters();
    if (_parametersValid) {
      height           = _height;
      threshold        = _thershold;
      sensitivity      = _sensitivity;
      _parametersValid = false;
      return true;
    }
    return false;
  }

  bool getRadarParameters(float& height, float& threshold,
                          uint32_t& sensitivity, float& rect_XL, float& rect_XR,
                          float& rect_ZF, float& rect_ZB) {
    getRadarParameters();
    if (_parametersValid) {
      height           = _height;
      threshold        = _thershold;
      sensitivity      = _sensitivity;
      rect_XL          = _rect_XL;
      rect_XR          = _rect_XR;
      rect_ZF          = _rect_ZF;
      rect_ZB          = _rect_ZB;
      _parametersValid = false;
      return true;
    }
    return false;
  }

  bool getRadarParameters(void) {
    uint16_t type = static_cast<uint16_t>(TypeFallDetection::RadarParameters);
    if (this->SendFrame(type, nullptr, 0)) {
      return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
    }
    return false;
  }

  bool setSensitivity(
      uint32_t _sensitivity) {  // Range: 3~10 average of 3 frames of data?
    uint8_t data[sizeof(uint32_t)];
    uint32ToBytes(_sensitivity, data);
    uint16_t type = static_cast<uint16_t>(TypeFallDetection::FallSensitivity);
    if (this->SendFrame(type, data, sizeof(data))) {
      return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
    }
    return false;
  }

  /**
   * @brief Set the Fall Threshold object
   *
   * @param threshold
   * @note the default value is 0.6 m
   * @return true
   * @return false
   */
  bool setFallThreshold(float threshold) {
    uint8_t data[sizeof(float)];
    floatToBytes(threshold, data);
    uint16_t type = static_cast<uint16_t>(TypeFallDetection::FallThreshold);
    if (this->SendFrame(type, data, sizeof(data))) {
      return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
    }
    return false;
  }

  /**
   * @attention setting range is 0.3~1.5
   */
  bool setAlamAreaParameters(float rect_XL, float rect_XR, float rect_ZF,
                             float rect_ZB) {
    uint8_t data[sizeof(float) * 4];
    floatToBytes(rect_XL, data);
    floatToBytes(rect_XR, data + sizeof(float));
    floatToBytes(rect_ZF, data + 2 * sizeof(float));
    floatToBytes(rect_ZB, data + 3 * sizeof(float));
    uint16_t type = static_cast<uint16_t>(TypeFallDetection::AlarmParameters);
    if (this->SendFrame(type, data, sizeof(data))) {
      return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
    }
    return false;
  }

  bool resetDevice(void) {
    uint16_t type = static_cast<uint16_t>(TypeFallDetection::RadarInitSetting);
    if (this->SendFrame(type, nullptr, 0)) {
      return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
    }
    return false;
  }

  bool get3DPointCloud(
      int option)  // option 0: detection results, 1: target information;
  {
    uint16_t type = option == 0
                        ? static_cast<uint16_t>(
                              TypeFallDetection::Report3DPointCloudDetection)
                        : static_cast<uint16_t>(
                              TypeFallDetection::Report3DPointCloudTartgetInfo);
    if (this->SendFrame(type, nullptr, 0)) {
      return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
    }
  }

  bool getHuman(bool& human_status) {
    human_status = _isHuman;
    return _isHuman;
  }

  // bool UserLogInfo(uint32_t OnOff) {
  //     uint8_t data[sizeof(uint32_t)];
  //     uint32ToBytes(OnOff, data);
  //     uint16_t type = static_cast<uint16_t>(TypeFallDetection::UserLogInfo);
  //     if (this->SendFrame(type, data, sizeof(uint32_t))) {
  //         return fetch(type, 1000);  // Timeout of 1000ms to fetch the
  //         response
  //     }
  // }
};
#endif /*SEEED_XIAO_MMWAVE_H*/