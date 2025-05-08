// TODO the opimizations 
#include <array>
#include <string_view>
#include <iostream>
#include <cmath>
#include "calories.h"
#include "constants.h"

//Functions for all Calories calculations

//(BMR) Total calories required per day
float calBMR(
    const float weight,
    const float height,
    uint16_t age,
    uint16_t gender
) {
    switch (static_cast<Gender>(gender))
    {
    case Gender::Male:
        return static_cast<float>((10 * weight) + (6.25 * height * 100) - (5 * age) + 5);

    case Gender::Female:
        return static_cast<float>((10 * weight) + (6.25 * height * 100) - (5 * age) - 161);

    case Gender::Others:
        return static_cast<float>((10 * weight) + (6.25 * height * 100) - (5 * age) + 5);

    default:
        return -1;
    }
}

//Body Mass Index
float calBMI(
    const float weight,
    const float height
) {
    float bmi = weight / static_cast<float>(std::pow(height, 2));
    return bmi;
}

//MET stands for metabolic equivalet of a task
float getMET(
    const std::array<std::pair<const char*, float>, 8>& METtable,
    const std::string_view activity
) {

    for (const auto& pair : METtable) {
        if (activity == pair.first) {
            return pair.second;
        }
    }

    std::cout << "Unrecognized activity\n";
    return -1;

}

float calBurnt(
    const float weight,
    const float time,
    const std::string_view activity
) {
    if (time == 0.0) {
        return -1;
    }

    auto met = getMET(constants::Activity_METtable, activity);
    if (met == -1) {
        return -1;
    }

    float caloriesBurnt = (time * met * weight) / 200;

    return caloriesBurnt;
}

float calFitnessScore(
    float user_bmr,
    float user_total_cal_burned
) {
    return user_total_cal_burned / user_bmr;
}