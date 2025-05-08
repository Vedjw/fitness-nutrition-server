#include <iostream>
#include "bcrypt/BCrypt.hpp"

int main() {
    std::string pass = "yolo";
    std::string hash = BCrypt::generateHash(pass, 12);
    std::cout << hash << '\n';
    std::cout << BCrypt::validatePassword(pass, hash) << '\n';
    return 0;
}