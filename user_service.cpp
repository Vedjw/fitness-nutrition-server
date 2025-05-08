
#include "User.h"
#include "user_service.h"
#include "calories.h"
#include <nlohmann/json.hpp>
#include <memory>
#include "Session.h"
#include "Error.h"
#include "Store.h"
#include <chrono>
#include "Ringbuf.h"
#include "handle_request_functions.h"
#include "calories.h"

extern Store g_DB;
extern RingBuf g_ring_buf;

struct Calories_consumed {
    float total_cal_consumed;
    float proteins;
    float carbs;
    float fats;
};

Error make_user(
    std::string& cookie_str,
    const std::string& data
) {
    using json = nlohmann::json;

    Error err = {
        .success = false,
        .message = "none"
    };

    long long new_user_id = std::chrono::steady_clock::now().time_since_epoch().count();

    try {
        json dataObj = json::parse(data);

        if (!dataObj.contains("User") || !dataObj["User"].is_object()) {
            err.message = "Incorrect json format";
            return err;
        }

        auto user = std::make_unique<User>(
            new_user_id,
            dataObj["User"].at("Fullname").get<std::string>(),
            dataObj["User"].at("Gender").get<uint16_t>(),
            dataObj["User"].at("Age").get<int>(),
            dataObj["User"].at("Height").get<float>(),
            dataObj["User"].at("Weight").get<float>(),
            dataObj["User"].at("Weight_goal").get<uint16_t>()
        );

        AuthInfo user_auth_info = {
            .email = dataObj["User"].at("Email").get<std::string>(),
            .password = dataObj["User"].at("Password").get<std::string>()
        };

        std::cout << "From outside the ring_buf" << "\n";
        user->print();

        g_DB.store_new_user(*user, user_auth_info);
        g_ring_buf.cache_user(get_cookie(cookie_str), std::move(user));

    }
    catch (json::exception json_execption) {
        std::cout << json_execption.what() << '\n'; // currently cout but later log
        throw "Unexpected server error";
    }

    err.success = true;
    return err;
}

Error auth_user(
    std::string& cookie_str,
    const std::string& data
) {
    using json = nlohmann::json;

    Error err = {
        .success = false,
        .message = "none"
    };

    try {
        json dataObj = json::parse(data);

        if (!dataObj.contains("UserAuthInfo") || !dataObj["UserAuthInfo"].is_object()) {
            err.message = "Incorrect json format";
            return err;
        }

        AuthInfo user_auth_info = {
            .email = dataObj["UserAuthInfo"].at("Email").get<std::string>(),
            .password = dataObj["UserAuthInfo"].at("Password").get<std::string>()
        };
        std::cout << user_auth_info.email << " " << user_auth_info.password << '\n';

        long long user_id = g_DB.authenticate_user(user_auth_info);
        std::cout << user_id << '\n';

        if (user_id == 0) {
            err.success = false;
            err.message = "Invalid email or password";
            return err;
        }

        auto user = std::move(g_DB.get_user_by_id(user_id));
        std::cout << cookie_str << '\n';//

        g_ring_buf.cache_user(get_cookie(cookie_str), std::move(user)); //debug here

    }
    catch (json::exception json_execption) {
        std::cout << json_execption.what() << '\n'; // currently cout but later log
        throw "Unexpected server error";
    }

    err.success = true;
    return err;
}

Error user_fitness_tracking(
    std::string& cookie_str,
    const std::string& data
) {
    std::cout << "from handle_tracking" << '\n';
    using json = nlohmann::json;

    Error err = {
        .success = false,
        .message = "none"
    };

    try {
        json dataObj = json::parse(data);

        if (!dataObj.contains("total_nutrition") || !dataObj["total_nutrition"].is_object()) {
            err.message = "Incorrect json format";
            return err;
        }

        auto user = g_ring_buf.find(get_cookie(cookie_str));
        if (!user) {
            err.success = false;
            err.message = "User not in Cache";
            std::cout << err.message << '\n';
            return err;
        }

        Calories_consumed user_cal_info = {
            .total_cal_consumed = dataObj["total_nutrition"].at("Calories").get<float>(),
            .proteins = dataObj["total_nutrition"].at("Proteins").get<float>(),
            .carbs = dataObj["total_nutrition"].at("Carbohydrates").get<float>(),
            .fats = dataObj["total_nutrition"].at("Fats").get<float>(),
        };
        float user_cal_burned{ 0 };

        if (dataObj["user_fitness"].is_array()) {
            for (const auto& item : dataObj["user_fitness"]) {
                user_cal_burned += calBurnt(
                    user->getWeight(),
                    item.at("Time").get<float>(),
                    item.at("ExerciseName").get<std::string_view>()
                );
            }
        }
        float user_bmr = calBMR(user->getWeight(), user->getHeight(), user->getAge(), user->getGender());
        float user_fitness_score = calFitnessScore(user_bmr, user_cal_burned);
        user->setCalConsumed(user_cal_info.total_cal_consumed);
        user->setCalBurned(user_cal_burned);
        user->print();
    }
    catch (json::exception json_execption) {
        std::cout << json_execption.what() << '\n'; // currently cout but later log
        throw "Unexpected server error";
    }
    err.success = true;
    return err;
}
