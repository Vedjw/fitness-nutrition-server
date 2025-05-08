#include <iostream>
#include "User.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <chrono>

int main() {
    using json = nlohmann::json;

    json data;

    data["firstName"] = "Vlad";
    data["lastName"] = "Tepes";
    data["Gender"] = Gender::Male;
    data["age"] = 23;
    data["height"] = 1.77f;
    data["weight"] = 60.0f;

    auto str = data.dump();

    json dataObj = json::parse(str);

    // std::string f_name{ dataObj["firstName"] };
    // float height = dataObj["height"];
    // std::string l_name{ dataObj["lastName"] };
    // Gender mygender{ dataObj["Gender"] };

    // auto user1 = std::make_unique<User>(f_name, l_name, mygender, 23, height, 60.0f);
    try {
        auto user = std::make_unique<User>(
            dataObj["First_Name"],
            dataObj["lastName"],
            dataObj["Gender"],
            dataObj["age"],
            dataObj["height"],
            dataObj["weight"]
        );
        user->print();
    }
    catch (json::exception jsonex) {
        std::cout << jsonex.what() << '\n';
    }

    std::cout << std::chrono::steady_clock::now().time_since_epoch().count() << '\n';

    return 0;

}