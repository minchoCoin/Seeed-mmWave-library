/**
 * @file SEEED_MR60FDC1.h
 * @date  02 July 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef SEEED_MR60FDC1_H
#define SEEED_MR60FDC1_H

#include "SeeedmmWave.h"

enum class TypeFallDetection : uint16_t {
  UserLogInfo = 0x0E01,

  ReportFallDetection = 0x0E02,  // is_fall
  InstallationHeight  = 0x0E04,
  RadarParameters     = 0x0E06,
  FallThreshold       = 0x0E08,
  FallSensitivity     = 0x0E0A,

  HeightUpload                  = 0x0E0E,
  AlarmParameters               = 0x0E0C,
  RadarInitSetting              = 0x2110,
  Report3DPointCloudDetection   = 0x0A08,
  Report3DPointCloudTartgetInfo = 0x0A04,
  ReportUnmannedDetection       = 0x0F09,
};

class SEEED_MR60FDC1 : public SeeedmmWave {
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
  SEEED_MR60FDC1() {}

  virtual ~SEEED_MR60FDC1() {}

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

  bool setInstallationHeight(float height);
  bool getFall(bool& fall_status);
  bool getRadarParameters(float& height, float& threshold,
                          uint32_t& sensitivity);
  bool getRadarParameters(float& height, float& threshold,
                          uint32_t& sensitivity, float& rect_XL, float& rect_XR,
                          float& rect_ZF, float& rect_ZB);
  bool getRadarParameters();
  bool setSensitivity(uint32_t _sensitivity);
  bool setFallThreshold(float threshold);
  bool setAlamAreaParameters(float rect_XL, float rect_XR, float rect_ZF,
                             float rect_ZB);
  bool resetDevice();
  bool get3DPointCloud(int option);
  bool getHuman(bool& human_status);
};

#endif /*SEEED_MR60FDC1_H*/