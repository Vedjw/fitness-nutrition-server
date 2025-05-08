#pragma once

#include <sqlite3.h>
#include <iostream>
#include "User.h"
#include <vector>

class Store {

    sqlite3* m_db;



public:

    Store() {
        auto err = db_init();
        if (err) {
            std::cerr << err << '\n';
        }
    }

    ~Store() {
        sqlite3_close(m_db);
    }

    bool store_new_user(
        const User& user,
        const AuthInfo& user_auth_info
    );
    std::unique_ptr<User> get_user_by_id(long long user_id);
    bool delete_user(const User& user);
    long long authenticate_user(const AuthInfo& user_auth_info);
    std::vector<float> get_user_scores(const User& user);
    bool update_user_score(const User& user);


private:

    bool db_init() {
        char* zErrMsg = 0;
        int rc = sqlite3_open("fitnessDB.db", &m_db);

        if (rc) {
            fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(m_db));
            std::exit(0);
        }
        else {
            fprintf(stderr, "Opened database successfully\n");
        }

        sqlite3_exec(m_db, "PRAGMA foreign_keys = ON;", 0, 0, &zErrMsg);

        return true;
    }

    bool add_user_auth(
        const AuthInfo& user_auth_info,
        long long u_id
    );
};