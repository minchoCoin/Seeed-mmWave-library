/**
 * @file mmWave.h
 * @date  09 May 2024

 * @author Spencer Yan
 *
 * @note Description of the file
 *
 * @copyright © 2024, Seeed Studio
 */

#ifndef XIAO_MMWAVEBREATHTYPES_H
#define XIAO_MMWAVEBREATHTYPES_H
#include <Arduino.h>
#include <stdint.h>

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

enum class TypeHeartBreath : uint16_t {
  TypeHeartBreathPhase    = 0x0A13,
  TypeBreathRate          = 0x0A14,
  TypeHeartRate           = 0x0A15,
  TypeHeartBreathDistance = 0x0A16,
};

class BreathData {
 public:
  virtual ~BreathData() = default;
  bool isValid() const {
    return valid;
  }
  bool isUpdated() const {
    return updated;
  }

 protected:
  mutable bool valid   = false;
  mutable bool updated = false;
};

class HeartBreath : public BreathData {
  float total_phase;
  float breath_phase;
  float heart_phase;

 public:
  HeartBreath(float tp, float bp, float hp)
      : total_phase(tp), breath_phase(bp), heart_phase(hp) {
    valid   = total_phase <= 0 || breath_phase <= 0 || heart_phase <= 0 ? false
                                                                        : true;
    updated = true;
  }

  TypeHeartBreath getType() const {
    return TypeHeartBreath::TypeHeartBreathPhase;
  }

  void getPhase(float& total, float& breath, float& heart) const {
    total  = total_phase;
    breath = breath_phase;
    heart  = heart_phase;

    // Explicitly allowed because these members are mutable
    valid = updated = false;
  }

  // Pure const method, does not modify the object
  std::tuple<float, float, float> getPhase() const {
    return std::make_tuple(total_phase, breath_phase, heart_phase);
  }
};

class BreathRate : public BreathData {
  float breath_rate;

 public:
  BreathRate(float br) : breath_rate(br) {
    valid   = breath_rate <= 0 ? false : true;
    updated = true;
  }
  TypeHeartBreath getType() const {
    return TypeHeartBreath::TypeBreathRate;
  }
  void getBreathRate(float& breath) const {
    breath = breath_rate;
    valid = updated = false;
  }
};

class HeartRate : public BreathData {
  float heart_rate;

 public:
  HeartRate(float hr) : heart_rate(hr) {
    valid   = heart_rate <= 0 ? false : true;
    updated = true;
  }
  TypeHeartBreath getType() const {
    return TypeHeartBreath::TypeHeartRate;
  }
  void getHeartRate(float& heart) const {
    heart = heart_rate;
    valid = updated = false;
  }
};

class HeartBreathDistance : public BreathData {
  uint32_t flag;
  float range;

 public:
  HeartBreathDistance(uint32_t f, float r) : flag(f), range(r) {
    valid   = *(bool*)&flag;
    updated = true;
  }
  TypeHeartBreath getType() const {
    return TypeHeartBreath::TypeHeartBreathDistance;
  }
  void getDistance(float& distance) const {
    distance = range;
    valid = updated = false;
  }
};

#endif /*XIAO_MMWAVEBREATHTYPES_H*/
