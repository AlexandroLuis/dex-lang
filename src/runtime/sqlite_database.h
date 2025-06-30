// src/runtime/sqlite_database.h
#ifndef DEX_SQLITE_DATABASE_H
#define DEX_SQLITE_DATABASE_H

#include "database.h"
#include <sqlite3.h>

namespace dex {

class SQLiteDatabase : public Database {
public:
    SQLiteDatabase();
    ~SQLiteDatabase() override;

    bool connect(const std::string& connStr) override;
    bool execute(const std::string& query) override;
    QueryResult query(const std::string& query) override;
    void close() override;

private:
    sqlite3* db = nullptr;
};

} // namespace dex

#endif
