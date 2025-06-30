// src/runtime/database.cpp
#include "database.h"
#include "sqlite_database.h"
#include "mysql_database.h"
#include "postgres_database.h"
#include <string>

namespace dex {

std::unique_ptr<Database> createDatabase(const std::string& connStr) {
    if (connStr.rfind("sqlite://", 0) == 0) {
        return std::make_unique<SQLiteDatabase>();
    }
    if (connStr.rfind("mysql://", 0) == 0) {
        return std::make_unique<MySQLDatabase>();
    }
    if (connStr.rfind("postgresql://", 0) == 0) {
        return std::make_unique<PostgresDatabase>();
    }
    return nullptr;
}

} // namespace dex
