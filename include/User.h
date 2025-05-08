#pragma once

#include <string>
#include <iostream>


enum class Gender {
    Invalid,
    Male,
    Female,
    Others
};

enum class Weight_Goal {
    Invalid,
    Maintain,
    Gain,
    Lose
};

struct AuthInfo {
    std::string email;
    std::string password;
};

class User : public std::enable_shared_from_this<User> {
private:

    long long m_id; //default initialization, because performance gains.
    std::string m_fullname;
    uint16_t m_gender;
    uint16_t m_age;
    float m_height;
    float m_weight;
    uint16_t m_goal;
    float m_current_score{ 0 };
    float m_cal_burned{ 0 };
    float m_cal_consumed{ 0 };
    std::vector<float> m_score_arr;

public:

    //Constructor
    User() = delete;
    User(const User& user) = delete;
    User(long long id,
        const std::string& fullname,
        uint16_t gender = 0,
        uint16_t age = 0,
        float height = 0.0f,
        float weight = 0.0f,
        uint16_t goal = 0
    );//right most args can have default values.

    //Destructor
    ~User() {
        std::cout << "User object destroyed" << '\n';
    }


    //Access member functions
    uint16_t getGender() const { return m_gender; }
    uint16_t getGoal() const { return m_goal; }
    uint16_t getAge() const { return m_age; }
    float getHeight() const { return m_height; }
    float getWeight() const { return m_weight; }
    std::vector<float> getScores() const { return m_score_arr; };
    float getCalBurned() const { return m_cal_burned; };
    float getCalConsumed() const { return m_cal_consumed; };
    void setScores(std::vector<float> user_scores_arr) {
        m_score_arr = user_scores_arr;
    }
    void setCalBurned(const float calBurnt) {
        m_cal_burned = calBurnt;
    }
    void setCalConsumed(const float calConsumed) {
        m_cal_consumed = calConsumed;
    }

    void print() const;

    friend class Store;
};

