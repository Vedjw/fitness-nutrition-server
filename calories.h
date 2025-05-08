#pragma once

#include "User.h"
#include <array>

float calBMR(
    const float weight,
    const float height,
    uint16_t age,
    uint16_t gender
);

float calBMI(
    const float weight,
    const float height
);

float getMET(const std::array<std::pair<const char*, float>, 8>& METtable,
    const std::string_view activity);

float calBurnt(
    const float weight,
    const float time,
    const std::string_view activity
);

float calFitnessScore(
    float user_bmr,
    float user_total_cal_burned
);