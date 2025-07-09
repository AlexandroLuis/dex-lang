// src/runtime/postgres_database.cpp
#include "postgres_database.h"
#include <iostream>
#include <regex>

namespace dex {

PostgresDatabase::PostgresDatabase() : conn(nullptr) {}

PostgresDatabase::~PostgresDatabase() {
    close();
}

bool PostgresDatabase::connect(const std::string& connStr) {
    // Example connStr: "postgresql://user:pass@host:port/dbname"
    try {
        std::regex rx(R"(postgresql://([^:]+):([^@]+)@([^:/]+):(\d+)/(.+))");
        std::smatch match;
        if (!std::regex_match(connStr, match, rx) || match.size() != 6) {
            std::cerr << "Postgres connection string format error\n";
            return false;
        }
        std::string user = match[1];
        std::string pass = match[2];
        std::string host = match[3];
        std::string port = match[4];
        std::string dbname = match[5];

        std::string pqconn = "user=" + user + " password=" + pass + " host=" + host +
                             " port=" + port + " dbname=" + dbname;

        conn = std::make_unique<pqxx::connection>(pqconn);
        if (!conn->is_open()) {
            std::cerr << "Failed to open Postgres connection\n";
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Postgres connection error: " << e.what() << "\n";
        return false;
    }
}

bool PostgresDatabase::execute(const std::string& query) {
    try {
        pqxx::work txn(*conn);
        txn.exec(query);
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Postgres execute error: " << e.what() << "\n";
        return false;
    }
}

QueryResult PostgresDatabase::query(const std::string& query) {
    QueryResult results;
    try {
        pqxx::work txn(*conn);
        pqxx::result res = txn.exec(query);

        for (auto row : res) {
            std::vector<std::string> rowData;
            for (auto field : row) {
                rowData.push_back(field.c_str() ? field.c_str() : "");
            }
            results.push_back(rowData);
        }
    } catch (const std::exception& e) {
        std::cerr << "Postgres query error: " << e.what() << "\n";
    }
    return results;
}

void PostgresDatabase::close() {
    if (conn) {
        conn->disconnect();
        conn.reset();
    }
}

} // namespace dex
