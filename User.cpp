#include <iostream>
#include "User.h"

//non-trivial member functions

User::User(
    long long id,
    const std::string& fullname,
    uint16_t gender,
    uint16_t age,
    float height,
    float weight,
    uint16_t goal
)
    : m_id{ id }
    , m_fullname{ fullname }
    , m_gender{ gender }
    , m_age{ age }
    , m_height{ height }
    , m_weight{ weight }
    , m_goal{ goal }
{
}

void User::print() const {
    std::cout << "User" << '\n';
    std::cout << '\t' << "id: " << m_id << '\n';
    std::cout << '\t' << "fullname: " << m_fullname << '\n';
    std::cout << '\t' << "gender: " << static_cast<int> (m_gender) << '\n';
    std::cout << '\t' << "age: " << m_age << '\n';
    std::cout << '\t' << "height: " << m_height << '\n';
    std::cout << '\t' << "weight: " << m_weight << '\n';
    std::cout << '\t' << "weight goal: " << static_cast<int> (m_goal) << '\n';
    std::cout << '\t' << "current score: " << m_current_score << '\n';
    std::cout << '\t' << "cal burned: " << m_cal_burned << '\n';
}
