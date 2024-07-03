/**
 * @file SEEED_MR60FDC1.cpp
 * @date  02 July 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#include "SEEED_MR60FDC1.h"

bool SEEED_MR60FDC1::resetDevice() {
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::RadarInitSetting);
  if (this->sendFrame(type, nullptr, 0)) {
    return fetch(type, 3000);  // Timeout of 3000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::setInstallationHeight(float height) {
  uint8_t data[sizeof(float)];
  floatToBytes(height, data);
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::InstallationHeight);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 3000);  // Timeout of 3000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::setSensitivity(uint32_t _sensitivity) {
  uint8_t data[sizeof(uint32_t)];
  uint32ToBytes(_sensitivity, data);
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::FallSensitivity);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 3000);  // Timeout of 3000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::setFallThreshold(float threshold) {
  uint8_t data[sizeof(float)];
  floatToBytes(threshold, data);
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::FallThreshold);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 3000);  // Timeout of 3000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::setAlamAreaParameters(float rect_XL, float rect_XR,
                                           float rect_ZF, float rect_ZB) {
  uint8_t data[sizeof(float) * 4];
  floatToBytes(rect_XL, data);
  floatToBytes(rect_XR, data + sizeof(float));
  floatToBytes(rect_ZF, data + 2 * sizeof(float));
  floatToBytes(rect_ZB, data + 3 * sizeof(float));
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::AlarmParameters);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 3000);  // Timeout of 3000ms to fetch the response
  }
  return false;
}

/**
 * @brief get the basic Radar parameters
 *
 * @param height
 * @param threshold
 * @param sensitivity
 * @return true
 * @return false
 */
bool SEEED_MR60FDC1::getRadarParameters(float& height, float& threshold,
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

/**
 * @brief get whole Radar parameteres
 *
 * @param height
 * @param threshold
 * @param sensitivity
 * @param rect_XL
 * @param rect_XR
 * @param rect_ZF
 * @param rect_ZB
 * @return true
 * @return false
 */
bool SEEED_MR60FDC1::getRadarParameters(float& height, float& threshold,
                                        uint32_t& sensitivity, float& rect_XL,
                                        float& rect_XR, float& rect_ZF,
                                        float& rect_ZB) {
  this->getRadarParameters();
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

/**
 * @brief Trigger Rader to return the parameters
 *
 * @return true
 * @return false
 */
bool SEEED_MR60FDC1::getRadarParameters() {
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::RadarParameters);
  if (this->sendFrame(type, nullptr, 0)) {
    return fetch(type, 2000);  // Timeout of 2000 ms to fetch the response
  }
  return false;
}

/**
 * @brief send type, there is no data to package
 *
 * @param option
 * @return true
 * @return false
 */
bool SEEED_MR60FDC1::get3DPointCloud(int option) {
  uint16_t type = option == 0
                      ? static_cast<uint16_t>(
                            TypeFallDetection::Report3DPointCloudDetection)
                      : static_cast<uint16_t>(
                            TypeFallDetection::Report3DPointCloudTartgetInfo);
  if (this->sendFrame(type, nullptr, 0)) {
    return fetch(type, 2000);  // Timeout of 2000 ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::getFall(bool& fall_status) {
    fall_status    = _isFall;
    return true;
}

bool SEEED_MR60FDC1::getHuman(bool& human_status) {
  human_status = _isHuman;
  return _isHuman;
}

/**
 * @brief Get the information from Rader and parse them
 *
 * @param _type
 * @param data
 * @param data_len
 * @return true
 * @return false
 */
bool SEEED_MR60FDC1::handleType(uint16_t _type, const uint8_t* data,
                                size_t data_len) {
  TypeFallDetection type = static_cast<TypeFallDetection>(_type);
  switch (type) {
    case TypeFallDetection::ReportFallDetection:
      _isFall = *(const bool*)data;
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
      isThresholdValid = *(const uint8_t*)data;
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