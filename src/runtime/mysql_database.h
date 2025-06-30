// src/runtime/mysql_database.h
#ifndef DEX_MYSQL_DATABASE_H
#define DEX_MYSQL_DATABASE_H

#include "database.h"
#include <memory>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

namespace dex {

class MySQLDatabase : public Database {
public:
    MySQLDatabase();
    ~MySQLDatabase() override;

    bool connect(const std::string& connStr) override;
    bool execute(const std::string& query) override;
    QueryResult query(const std::string& query) override;
    void close() override;

private:
    sql::mysql::MySQL_Driver* driver = nullptr;
    std::unique_ptr<sql::Connection> conn;
};

} // namespace dex

#endif
