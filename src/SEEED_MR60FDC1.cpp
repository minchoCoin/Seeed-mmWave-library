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

/**
 * @brief Radar initialization.
 *
 * @details This function initializes the radar with specific settings.
 *
 * @attention The following parameters should be set:
 * - High: 2.2
 * - Threshold: 0.5
 * - Sensitivity: 3
 * - Rect_XL: 0.5
 * - Rect_XR: 0.5
 * - Rect_ZF: 0.5
 * - Rect_ZB: 0.5
 */
bool SEEED_MR60FDC1::resetSetting(void) {
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::RadarInitSetting);
  if (this->sendFrame(type, nullptr, 0)) {
    return fetch(type, 3000);  // Timeout of 3000ms to fetch the response
  }
  return false;
}

/**
 * @brief Set the radar installation height
 *
 * @param height 1 to 5 m
 * @retval true Set height successfully
 * @retval false Failed to set altitude
 *
 */
bool SEEED_MR60FDC1::setInstallationHeight(const float height) {
  uint8_t data[sizeof(float)];
  floatToBytes(height, data);
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::InstallationHeight);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 3000);  // Timeout of 3000ms to fetch the response
  }
  return false;
}

/**
 * @brief Set fall threshold
 *
 * @param threshold
 *
 * @note The default fall threshold of the radar is 0.6 m.
 */
bool SEEED_MR60FDC1::setThreshold(const float threshold) {
  uint8_t data[sizeof(float)];
  floatToBytes(threshold, data);
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::FallThreshold);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 40000);  // Timeout of 40000 ms to fetch the response
  }
  return false;
}

/**
 * @brief Set the fall sensitivity
 *
 * @param _sensitivity  range 3 to 10
 * @retval true Set successfully
 * @retval false Failed to set
 *
 * @note the initial value is 3, which represents an average of 3 frames of
 * data.
 */
bool SEEED_MR60FDC1::setSensitivity(const uint32_t _sensitivity) {
  uint8_t data[sizeof(uint32_t)];
  uint32ToBytes(_sensitivity, data);
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::FallSensitivity);
  if (this->sendFrame(type, data, sizeof(data))) {
    return fetch(type, 3000);  // Timeout of 3000ms to fetch the response
  }
  return false;
}

/**
 * @brief Set the Alam Area object
 *
 * @param rect_XL
 * @param rect_XR
 * @param rect_ZF
 * @param rect_ZB
 * @retval true Set successfully
 * @retval false Failed to set
 */
bool SEEED_MR60FDC1::setAlamArea(const float rect_XL, const float rect_XR,
                                 const float rect_ZF, const float rect_ZB) {
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
 * @retval true acquisition is successful
 * @retval false failed to obtain
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
 * @retval true acquisition is successful
 * @retval false failed to obtain
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
 * @retval true acquisition is successful
 * @retval false failed to obtain
 */
bool SEEED_MR60FDC1::getRadarParameters() {
  uint16_t type = static_cast<uint16_t>(TypeFallDetection::RadarParameters);
  if (this->sendFrame(type, nullptr, 0)) {
    return fetch(type, 3000);  // Timeout of 2000 ms to fetch the response
  }
  return false;
}

/**
 * @brief Determine whether a fall has been detected.
 *
 * @param fall_status A reference to a boolean variable where the fall status
 * will be stored.
 *                    - `true` indicates a fall has been detected.
 *                    - `false` indicates no fall has been detected.
 *
 * @retval true The function executed successfully and the fall status has been
 * updated.
 * @retval false The function failed to execute.
 */
bool SEEED_MR60FDC1::getFall(bool& fall_status) {
  fall_status = _isFall;
  return _isFall;
}

/**
 * @brief Determine whether a human is present.
 *
 * @param human_status A reference to a boolean variable where the human
 * presence status will be stored.
 *                     - `false` indicates no human is present.
 *                     - `true` indicates a human is present.
 *
 * @retval true A human is detected.
 * @retval false No human is detected.
 */
bool SEEED_MR60FDC1::getHuman(bool& human_status) {
  human_status = _isHuman;
  return _isHuman;
}

/**
 * @brief Handle different types of fall detection data.
 *
 * This function processes different types of fall detection data based on the
 * type identifier.
 *
 * @param _type The type identifier of the data.
 * @param data The pointer to the data buffer.
 * @param data_len The length of the data buffer.
 * @retval true if the data is handled successfully.
 * @retval false if there is an error in handling the data.
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