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

class SEEED_MR60BHA2 : public SeeedmmWave {
 private:
  HeartBreath* _heartBreath;
  BreathRate* _breathRate;
  HeartRate* _heartRate;
  HeartBreathDistance* _heartBreathDistance;

 public:
  SEEED_MR60BHA2()
      : _heartBreath(nullptr),
        _breathRate(nullptr),
        _heartRate(nullptr),
        _heartBreathDistance(nullptr) {}

  virtual ~SEEED_MR60BHA2() {
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
                            float& heart_phase) const;
  bool getBreathRate(float& rate) const;
  bool getHeartRate(float& rate) const;
  bool getHeartBreathDistance(float& distance) const;
};

#endif /*SEEED_MR60BHA2_H*/