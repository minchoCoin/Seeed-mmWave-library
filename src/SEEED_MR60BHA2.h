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

enum class TypeHeartBreath : uint16_t {
  TypeHeartBreathPhase    = 0x0A13,
  TypeBreathRate          = 0x0A14,
  TypeHeartRate           = 0x0A15,
  TypeHeartBreathDistance = 0x0A16,
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
};

#endif /*SEEED_MR60BHA2_H*/