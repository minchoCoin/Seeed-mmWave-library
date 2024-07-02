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

bool SEEED_MR60FDC1::setInstallationHeight(float height) {
  uint8_t data[sizeof(float)];
  floatToBytes(height, data);
  uint16_t type =
      static_cast<uint16_t>(TypeFallDetection::InstallationHeight);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::getFall(bool& fall_status) {
  if (_isFallUpdated) {
    fall_status    = _isFall;
    _isFallUpdated = false;
    return true;
  }
  return false;
}

bool SEEED_MR60FDC1::getRadarParameters(float& height, float& threshold, uint32_t& sensitivity) {
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

bool SEEED_MR60FDC1::getRadarParameters(float& height, float& threshold, uint32_t& sensitivity,
                          float& rect_XL, float& rect_XR, float& rect_ZF, float& rect_ZB) {
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

bool SEEED_MR60FDC1::getRadarParameters() {
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::RadarParameters);
  if (this->sendFrame(type, nullptr, 0)) {
    return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::setSensitivity(uint32_t _sensitivity) {
  uint8_t data[sizeof(uint32_t)];
  uint32ToBytes(_sensitivity, data);
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::FallSensitivity);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::setFallThreshold(float threshold) {
  uint8_t data[sizeof(float)];
  floatToBytes(threshold, data);
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::FallThreshold);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::setAlamAreaParameters(float rect_XL, float rect_XR, float rect_ZF, float rect_ZB) {
  uint8_t data[sizeof(float) * 4];
  floatToBytes(rect_XL, data);
  floatToBytes(rect_XR, data + sizeof(float));
  floatToBytes(rect_ZF, data + 2 * sizeof(float));
  floatToBytes(rect_ZB, data + 3 * sizeof(float));
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::AlarmParameters);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::resetDevice() {
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::RadarInitSetting);
  if (this->sendFrame(type, nullptr, 0)) {
    return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::get3DPointCloud(int option) {
  uint16_t type = option == 0
                      ? static_cast<uint16_t>(
                            TypeFallDetection::Report3DPointCloudDetection)
                      : static_cast<uint16_t>(
                            TypeFallDetection::Report3DPointCloudTartgetInfo);
  if (this->sendFrame(type, nullptr, 0)) {
    return fetch(type, 1000);  // Timeout of 1000ms to fetch the response
  }
  return false;
}

bool SEEED_MR60FDC1::getHuman(bool& human_status) {
  human_status = _isHuman;
  return _isHuman;
}
