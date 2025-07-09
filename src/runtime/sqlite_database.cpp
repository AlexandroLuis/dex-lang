// src/runtime/sqlite_database.cpp
#include "sqlite_database.h"
#include <iostream>
#include <cstring>

namespace dex {

SQLiteDatabase::SQLiteDatabase() : db(nullptr) {}

SQLiteDatabase::~SQLiteDatabase() {
    close();
}

bool SQLiteDatabase::connect(const std::string& connStr) {
    // connStr format: "sqlite://path/to/file.db"
    std::string filename = connStr.substr(strlen("sqlite://"));
    int rc = sqlite3_open(filename.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "SQLite open error: " << sqlite3_errmsg(db) << "\n";
        return false;
    }
    return true;
}

bool SQLiteDatabase::execute(const std::string& query) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQLite execute error: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

QueryResult SQLiteDatabase::query(const std::string& query) {
    QueryResult results;
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "SQLite prepare failed\n";
        return results;
    }

    int cols = sqlite3_column_count(stmt);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::vector<std::string> row;
        for (int i = 0; i < cols; ++i) {
            const unsigned char* text = sqlite3_column_text(stmt, i);
            row.emplace_back(text ? reinterpret_cast<const char*>(text) : "");
        }
        results.push_back(row);
    }

    sqlite3_finalize(stmt);
    return results;
}

void SQLiteDatabase::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

} // namespace dex
