/**
 * @file SEEED_MR60BHA2.h
 * @date  02 July 2024
 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 *
 * @attention MR60BHA2 module is used to monitor breath and heart rate
 */

#ifndef SEEED_MR60BHA2_H
#define SEEED_MR60BHA2_H

#include "SeeedmmWave.h"
#include "SEEED_Public.h"
#define MAX_TARGET_NUM    3

#define RANGE_STEP 17.28f

enum class TypeHeartBreath : uint16_t {
  TypeHeartBreathPhase    = 0x0A13,
  TypeBreathRate          = 0x0A14,
  TypeHeartRate           = 0x0A15,
  TypeHeartBreathDistance = 0x0A16,
  Report3DPointCloudDetection   = 0x0A08,
  Report3DPointCloudTartgetInfo = 0x0A04,
  ReportHumanDetection       = 0x0F09,
};

typedef struct HeartBreath {
  float total_phase;
  float breath_phase;
  float heart_phase;
} HeartBreath;



class SEEED_MR60BHA2 : public SeeedmmWave {
 private:
  /* HeartBreath */
  HeartBreath _heart_breath = {0};

  /* BreathRate */
  float _breath_rate;

  /* HeartRate */
  float _heart_rate;

  /* HeartBreathDistance */
  uint32_t _rangeFlag;
  float _range;

  /* HumanDetection */
  bool _isHumanDetected;             // 0 : no one            1 : There is someone
  bool _isHumanDetectionValid;

  /* PeopleCounting PointCloud */
  PeopleCounting _people_counting_point_cloud;
  bool _isPeopleCountingPointCloudValid;
 
  /* PeopleCounting TartgetInfo */
  PeopleCounting _people_counting_target_info;
  bool _isPeopleCountingTartgetInfoValid;

  bool _isHeartBreathPhaseValid = false;
  bool _isBreathRateValid       = false;
  bool _isHeartRateValid        = false;
  bool _isDistanceValid         = false;

 public:
  SEEED_MR60BHA2() {}

  virtual ~SEEED_MR60BHA2() {}

  bool handleType(uint16_t _type, const uint8_t* data,
                  size_t data_len) override;

  bool getHeartBreathPhases(float& total_phase, float& breath_phase,
                            float& heart_phase);
  bool getBreathRate(float& rate);
  bool getHeartRate(float& rate);
  bool getDistance(float& distance);
  bool getPeopleCountingPointCloud(PeopleCounting& point_cloud);
  bool getPeopleCountingTartgetInfo(PeopleCounting& target_info);
  bool isHumanDetected();
};

#endif /*SEEED_MR60BHA2_H*/