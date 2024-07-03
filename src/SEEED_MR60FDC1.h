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
  /* get fall detection*/
  bool _isFall = false;

  /*set height*/
  bool _HightValid = false;

  /*  get parameters */
  bool _parametersValid = false;
  float _height;
  float _thershold;  // deault value=0.6m
  uint32_t _sensitivity;
  float _rect_XL;
  float _rect_XR;
  float _rect_ZF;
  float _rect_ZB;

  bool isThresholdValid;     // 0 : Failed to obtain  1 : acquisition successful
  bool _isAlarmAreaValid;    // 0 : Failed to obtain  1 : acquisition
                             // successful
  bool _isSensitivityValid;  // 0 : Failed to obtain  1 : acquisition successful
  bool _isHuman;             // 0 : no one            1 : There is someone

 protected:
  bool getRadarParameters();

 public:
  SEEED_MR60FDC1() {}

  virtual ~SEEED_MR60FDC1() {}

  bool handleType(uint16_t _type, const uint8_t* data,
                  size_t data_len) override;

  bool resetSetting(void);

  bool setInstallationHeight(const float height);
  bool setThreshold(const float threshold);
  bool setSensitivity(const uint32_t _sensitivity);
  bool setAlamArea(const float rect_XL, const float rect_XR,
                   const float rect_ZF, const float rect_ZB);

  bool getRadarParameters(float& height, float& threshold,
                          uint32_t& sensitivity);
  bool getRadarParameters(float& height, float& threshold,
                          uint32_t& sensitivity, float& rect_XL, float& rect_XR,
                          float& rect_ZF, float& rect_ZB);

  // bool get3DPointCloud(const int option);

  bool getFall(bool& fall_status);
  bool getHuman(bool& human_status);
};

#endif /*SEEED_MR60FDC1_H*/