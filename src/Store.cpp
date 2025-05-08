#include "Store.h"
#include "bcrypt/BCrypt.hpp"
#include "User.h"
#include <memory>

bool Store::add_user_auth(
    const AuthInfo& user_auth_info,
    long long u_id
) {

    std::string new_password = BCrypt::generateHash(user_auth_info.password, 12);

    // Add user's auth info
    std::string sql_query_add_auth =
        "INSERT INTO AUTH_USER(user_id, email, password) VALUES (" +
        std::to_string(u_id) + ", '" +
        user_auth_info.email + "', '" +
        new_password + "');";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(m_db, sql_query_add_auth.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool Store::store_new_user(
    const User& user,
    const AuthInfo& user_auth_info
) {
    try {
        // Add new user
        std::string sql_query_insert =
            "INSERT INTO USERS(user_id, fullname, gender, age, height, weight, current_weight_goal) VALUES (" +
            std::to_string(user.m_id) + ", '" +
            user.m_fullname + "', " +
            std::to_string(user.m_gender) + ", " +
            std::to_string(user.m_age) + ", " +
            std::to_string(user.m_height) + ", " +
            std::to_string(user.m_weight) + ", " +
            std::to_string(user.m_goal) + ");";

        char* errMsg = nullptr;
        int rc = sqlite3_exec(m_db, sql_query_insert.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        if (!add_user_auth(user_auth_info, user.m_id)) {
            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception in store_new_user: " << ex.what() << std::endl;
        return false;
    }
}

bool Store::delete_user(const User& user) {
    try {

        std::string sqlquery = "DELETE FROM users WHERE user_id = " + std::to_string(user.m_id) + ";";

        char* errMsg = nullptr;
        int rc = sqlite3_exec(m_db, sqlquery.c_str(), nullptr, nullptr, &errMsg);

        if (rc != SQLITE_OK) {
            std::cerr << "SQL error while deleting user in users: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        std::string sqlquery2 = "DELETE FROM scores WHERE user_id = " + std::to_string(user.m_id) + ";";

        char* errMsg2 = nullptr;
        int rc2 = sqlite3_exec(m_db, sqlquery2.c_str(), nullptr, nullptr, &errMsg2);

        if (rc2 != SQLITE_OK) {
            std::cerr << "SQL error while deleting user in scores: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        std::string sqlquery3 = "DELETE FROM auth_user WHERE user_id = " + std::to_string(user.m_id) + ";";

        char* errMsg3 = nullptr;
        int rc3 = sqlite3_exec(m_db, sqlquery3.c_str(), nullptr, nullptr, &errMsg3);

        if (rc3 != SQLITE_OK) {
            std::cerr << "SQL error while deleting user in auth_user: " << errMsg << std::endl;
            sqlite3_free(errMsg);
            return false;
        }

        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception in delete_user: " << ex.what() << std::endl;
        return false;
    }
}

long long Store::authenticate_user(const AuthInfo& user_auth_info) {
    long long user_id{};
    std::string stored_password{};
    sqlite3_stmt* stmt;
    std::string sql_query = "SELECT user_id, password FROM AUTH_USER WHERE email = ?;";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(m_db, sql_query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return 0;
    }

    // Bind email parameter to the SQL statement
    if (sqlite3_bind_text(stmt, 1, user_auth_info.email.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Failed to bind email: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return 0;
    }

    // Execute and check result
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        user_id = sqlite3_column_int64(stmt, 0);
        stored_password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    }
    std::cout << stored_password << user_auth_info.password << '\n';
    std::cout << BCrypt::validatePassword(user_auth_info.password, stored_password) << '\n';
    std::cout << user_id << '\n';
    if (BCrypt::validatePassword(user_auth_info.password, stored_password)) {
        std::cout << "hi from here" << '\n';
        return user_id;
    }

    sqlite3_finalize(stmt);
    return 0;

}

std::vector<float> Store::get_user_scores(const User& user) {
    std::vector<float> scores;
    sqlite3_stmt* stmt;

    std::string sql_query = "SELECT score_value FROM SCORES WHERE user_id = ?;";

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(m_db, sql_query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return scores;
    }

    // Bind user_id
    if (sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(user.m_id)) != SQLITE_OK) {
        std::cerr << "Failed to bind user_id: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return scores;
    }

    // Fetch all matching rows
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int score = sqlite3_column_int(stmt, 0); // matches INTEGER type in schema
        scores.push_back(static_cast<float>(score)); // cast to float if needed
    }

    sqlite3_finalize(stmt);
    return scores;
}

std::unique_ptr<User> Store::get_user_by_id(long long user_id) {
    sqlite3_stmt* stmt;
    std::string sql_query = "SELECT user_id, fullname, gender, age, height, weight, current_weight_goal FROM USERS WHERE user_id = ? LIMIT 1;";

    if (sqlite3_prepare_v2(m_db, sql_query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return nullptr;
    }

    if (sqlite3_bind_int64(stmt, 1, static_cast<sqlite3_int64>(user_id)) != SQLITE_OK) {
        std::cerr << "Failed to bind user_id: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return nullptr;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto user = std::make_unique<User>(
            sqlite3_column_int64(stmt, 0),
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
            static_cast<uint16_t>(sqlite3_column_int(stmt, 2)),
            static_cast<uint16_t>(sqlite3_column_int(stmt, 3)),
            static_cast<float>(sqlite3_column_double(stmt, 4)),
            static_cast<float>(sqlite3_column_double(stmt, 5)),
            static_cast<uint16_t>(sqlite3_column_int(stmt, 6))
        );

        auto user_scores = get_user_scores(*user);
        user->print();
        user->setScores(user_scores);

        sqlite3_finalize(stmt);
        return user;
    }

    sqlite3_finalize(stmt);
    return nullptr;
}
