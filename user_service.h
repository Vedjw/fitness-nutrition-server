#pragma once


#include <string>
#include <nlohmann/json.hpp>
#include "Error.h"

Error make_user(
    std::string& cookie_str,
    const std::string& data
);

Error auth_user(
    std::string& cookie_str,
    const std::string& data
);

Error user_fitness_tracking(
    std::string& cookie_str,
    const std::string& data
);