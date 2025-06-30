// src/runtime/postgres_database.h
#ifndef DEX_POSTGRES_DATABASE_H
#define DEX_POSTGRES_DATABASE_H

#include "database.h"
#include <memory>
#include <pqxx/pqxx>

namespace dex {

class PostgresDatabase : public Database {
public:
    PostgresDatabase();
    ~PostgresDatabase() override;

    bool connect(const std::string& connStr) override;
    bool execute(const std::string& query) override;
    QueryResult query(const std::string& query) override;
    void close() override;

private:
    std::unique_ptr<pqxx::connection> conn;
};

} // namespace dex

#endif
