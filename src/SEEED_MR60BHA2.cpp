#include "SEEED_MR60BHA2.h"

/**
 * @brief Handle different types of heart and breath data.
 *
 * This function processes different types of heart and breath data based on
 * the type identifier. It first deletes any existing objects to avoid memory
 * leaks.
 *
 * @param _type The type identifier of the data.
 * @param data The pointer to the data buffer.
 * @param data_len The length of the data buffer.
 * @return true if the data is handled successfully.
 * @return false if there is an error in handling the data.
 */
bool SEEED_MR60BHA2::handleType(uint16_t _type, const uint8_t* data,
                                size_t data_len) {
  TypeHeartBreath type = static_cast<TypeHeartBreath>(_type);
  switch (type) {
    case TypeHeartBreath::TypeHeartBreathPhase: {
      _heart_breath.total_phase  = extractFloat(data);
      _heart_breath.breath_phase = extractFloat(data + sizeof(float));
      _heart_breath.heart_phase  = extractFloat(data + 2 * sizeof(float));
      _isHeartBreathPhaseValid   = true;
      break;
    }
    case TypeHeartBreath::TypeBreathRate: {
      _breath_rate       = extractFloat(data);
      _isBreathRateValid = true;
      break;
    }
    case TypeHeartBreath::TypeHeartRate: {
      _heart_rate       = extractFloat(data);
      _isHeartRateValid = true;
      break;
    }
    case TypeHeartBreath::TypeHeartBreathDistance: {
      _rangeFlag       = extractU32(data);
      _range           = extractFloat(data + sizeof(uint32_t));
      _isDistanceValid = true;
      break;
    }
    case TypeHeartBreath::ReportHumanDetection: {
      _isHumanDetected = data[0];
      _isHumanDetectionValid = true;
      break;
    }
    case TypeHeartBreath::Report3DPointCloudDetection: {
      _people_counting_point_cloud.target_num = extractU32(data);
      _people_counting_point_cloud.x_point = extractFloat(data + sizeof(uint32_t));
      _people_counting_point_cloud.y_point = extractFloat(data + sizeof(uint32_t) + sizeof(float));
      _people_counting_point_cloud.dop_index = extractU32(data + sizeof(uint32_t) + 2 * sizeof(float));
      _people_counting_point_cloud.cluster_index = extractU32(data + 2 * sizeof(uint32_t) + 2 * sizeof(float));
      _isPeopleCountingPointCloudValid = true;
      break;
    }
    case TypeHeartBreath::Report3DPointCloudTartgetInfo: {
      _people_counting_target_info.target_num = extractU32(data);
      _people_counting_target_info.x_point = extractFloat(data + sizeof(uint32_t));
      _people_counting_target_info.y_point = extractFloat(data + sizeof(uint32_t) + sizeof(float));
      _people_counting_target_info.dop_index = extractU32(data + sizeof(uint32_t) + 2 * sizeof(float));
      _people_counting_target_info.cluster_index = extractU32(data + 2 * sizeof(uint32_t) + 2 * sizeof(float));
      _isPeopleCountingTartgetInfoValid = true;
      break;
    }
    default:
      return false;  // Unhandled type
  }
  return true;
}

bool SEEED_MR60BHA2::getHeartBreathPhases(float& total_phase,
                                          float& breath_phase,
                                          float& heart_phase) {
  if (!_isHeartBreathPhaseValid)
    return false;
  _isHeartBreathPhaseValid = false;

  total_phase  = _heart_breath.total_phase;
  breath_phase = _heart_breath.breath_phase;
  heart_phase  = _heart_breath.heart_phase;
  return true;
}

bool SEEED_MR60BHA2::getBreathRate(float& rate) {
  if (!_isBreathRateValid)
    return false;
  _isBreathRateValid = false;
  rate               = _breath_rate;
  return true;
}

bool SEEED_MR60BHA2::getHeartRate(float& rate) {
  if (!_isHeartRateValid)
    return false;
  _isHeartRateValid = false;
  rate              = _heart_rate;
  return true;
}

bool SEEED_MR60BHA2::getDistance(float& distance) {
  if (!_isDistanceValid || !_rangeFlag)
    return false;
  _isDistanceValid = false;
  distance         = _range;
  return true;
}

bool SEEED_MR60BHA2::getPeopleCountingPointCloud(PeopleCounting& point_cloud) {
  if (!_isPeopleCountingPointCloudValid)
    return false;
  _isPeopleCountingPointCloudValid = false;
  point_cloud = _people_counting_point_cloud;
  return true;
}

bool SEEED_MR60BHA2::getPeopleCountingTartgetInfo(PeopleCounting& target_info) {
  if (!_isPeopleCountingTartgetInfoValid)
    return false;
  _isPeopleCountingTartgetInfoValid = false;
  target_info = _people_counting_target_info;
  return true;
}

bool SEEED_MR60BHA2::isHumanDetected() {
  if (!_isHumanDetectionValid)
    return false;
  _isHumanDetectionValid = false;
  return _isHumanDetected;
}
