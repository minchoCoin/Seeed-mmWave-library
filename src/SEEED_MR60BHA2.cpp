#include "SEEED_MR60BHA2.h"

bool SEEED_MR60BHA2::getHeartBreathPhases(float& total_phase, float& breath_phase, float& heart_phase) const {
  if (_heartBreath && _heartBreath->isValid()) {
    _heartBreath->getPhase(total_phase, breath_phase, heart_phase);
    return true;
  }
  return false;
}

bool SEEED_MR60BHA2::getBreathRate(float& rate) const {
  if (_breathRate && _breathRate->isValid()) {
    _breathRate->getBreathRate(rate);
    return true;
  }
  return false;
}

bool SEEED_MR60BHA2::getHeartRate(float& rate) const {
  if (_heartRate && _heartRate->isValid()) {
    _heartRate->getHeartRate(rate);
    return true;
  }
  return false;
}

bool SEEED_MR60BHA2::getHeartBreathDistance(float& distance) const {
  if (_heartBreathDistance && _heartBreathDistance->isValid()) {
    _heartBreathDistance->getDistance(distance);
    return true;
  }
  return false;
}
