#ifndef SEEED_PUBLIC_H
#define SEEED_PUBLIC_H

#include "SeeedmmWave.h"
typedef struct TargetN {
  float x_point;
  float y_point;
  int32_t dop_index;
  int32_t cluster_index;
} TargetN;

typedef struct PeopleCounting {
  std::vector<TargetN> targets;
} PeopleCounting;

#endif