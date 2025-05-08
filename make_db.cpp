#include <stdio.h>
#include <sqlite3.h> 
#include <array>

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    int i;
    for (i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int main(int argc, char* argv[]) {
    sqlite3* db;
    char* zErrMsg = 0;
    int rc;

    rc = sqlite3_open("fitnessDB.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return(0);
    }
    else {
        fprintf(stderr, "Opened database successfully\n");
    }

    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, &zErrMsg);

    const char* sql_users = "CREATE TABLE USERS("  \
        "user_id INTEGER PRIMARY KEY NOT NULL," \
        "fullname TEXT NOT NULL," \
        "gender INTEGER NOT NULL," \
        "age INTEGER," \
        "height REAL," \
        "weight REAL," \
        "current_weight_goal INTEGER," \
        "created_at DATETIME DEFAULT CURRENT_TIMESTAMP" \
        ");";

    const char* sql_scores = "CREATE TABLE SCORES("  \
        "score_id INTEGER PRIMARY KEY," \
        "user_id INTEGER NOT NULL," \
        "score_value INTEGER NOT NULL," \
        "recorded_at DATETIME DEFAULT CURRENT_TIMESTAMP," \
        "FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE" \
        ");";

    const char* sql_auth_user = "CREATE TABLE AUTH_USER("  \
        "user_id INTEGER PRIMARY KEY," \
        "email TEXT NOT NULL UNIQUE," \
        "password TEXT NOT NULL," \
        "FOREIGN KEY(user_id) REFERENCES users(user_id) ON DELETE CASCADE" \
        ");";

    /* Execute SQL statement */

    rc = sqlite3_exec(db, sql_users, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Table created successfully\n");
    }

    int rc2 = sqlite3_exec(db, sql_scores, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Table created successfully\n");
    }

    int rc3 = sqlite3_exec(db, sql_auth_user, callback, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        fprintf(stdout, "Table created successfully\n");
    }

    sqlite3_close(db);

}
