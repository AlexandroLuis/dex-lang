// src/runtime/database.h
#ifndef DEX_DATABASE_H
#define DEX_DATABASE_H

#include <string>
#include <vector>
#include <memory>

namespace dex {

using QueryResult = std::vector<std::vector<std::string>>;

class Database {
public:
    virtual ~Database() = default;

    // Connect to the database using connection string (format depends on DB type)
    virtual bool connect(const std::string& connStr) = 0;

    // Execute a query (no results expected)
    virtual bool execute(const std::string& query) = 0;

    // Execute a query and get results
    virtual QueryResult query(const std::string& query) = 0;

    // Close connection
    virtual void close() = 0;
};

// Factory method to create appropriate Database subclass
std::unique_ptr<Database> createDatabase(const std::string& connStr);

} // namespace dex

#endif
