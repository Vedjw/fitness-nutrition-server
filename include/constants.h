#pragma once

#include <array>
#include <string>
#include <utility>

namespace constants {

    inline constexpr std::array<std::pair<const char*, float>, 8> Activity_METtable = { {
        {"Walking Slow Pace", 2.8},
        {"Walking Moderate Pace", 3.5},
        {"Jogging", 7.0},
        {"Running", 8.3},
        {"Calisthenics", 3.8},
        {"Bicycling", 5.8},
        {"Weight Training", 5.0},
        {"Swimming", 5.8}
    } };

}