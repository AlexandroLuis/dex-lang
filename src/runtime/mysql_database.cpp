// src/runtime/mysql_database.cpp
#include "mysql_database.h"
#include <iostream>
#include <regex>

namespace dex {

MySQLDatabase::MySQLDatabase() : driver(nullptr) {}

MySQLDatabase::~MySQLDatabase() {
    close();
}

bool MySQLDatabase::connect(const std::string& connStr) {
    // Example connStr: "mysql://user:pass@host:port/dbname"
    try {
        driver = sql::mysql::get_mysql_driver_instance();

        // Simple regex to parse connStr (basic, improve for production)
        std::regex rx(R"(mysql://([^:]+):([^@]+)@([^:/]+):(\d+)/(.+))");
        std::smatch match;
        if (!std::regex_match(connStr, match, rx) || match.size() != 6) {
            std::cerr << "MySQL connection string format error\n";
            return false;
        }
        std::string user = match[1];
        std::string pass = match[2];
        std::string host = match[3];
        std::string port = match[4];
        std::string dbname = match[5];

        std::string url = "tcp://" + host + ":" + port;
        conn.reset(driver->connect(url, user, pass));
        conn->setSchema(dbname);
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "MySQL connection error: " << e.what() << "\n";
        return false;
    }
}

bool MySQLDatabase::execute(const std::string& query) {
    try {
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        stmt->execute(query);
        return true;
    } catch (sql::SQLException& e) {
        std::cerr << "MySQL execute error: " << e.what() << "\n";
        return false;
    }
}

Database::QueryResult MySQLDatabase::query(const std::string& query) {
    QueryResult results;
    try {
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query));
        int cols = res->getMetaData()->getColumnCount();

        while (res->next()) {
            std::vector<std::string> row;
            for (int i = 1; i <= cols; ++i) {
                row.push_back(res->getString(i));
            }
            results.push_back(row);
        }
    } catch (sql::SQLException& e) {
        std::cerr << "MySQL query error: " << e.what() << "\n";
    }
    return results;
}

void MySQLDatabase::close() {
    conn.reset();
}

} // namespace dex
