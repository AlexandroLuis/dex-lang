// src/runtime/dex_database_binding.cpp

#include "database.h"
#include "interpreter.h"  // Your interpreter context and types
#include <memory>

// Storage for the currently connected database object (simplify for demo)
static std::unique_ptr<dex::Database> currentDB = nullptr;

// Dex function: Database.connect(connStr)
Value dex_database_connect(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isString()) {
        throw std::runtime_error("Database.connect expects 1 string argument");
    }
    std::string connStr = args[0].asString();

    currentDB = dex::createDatabase(connStr);
    if (!currentDB) {
        throw std::runtime_error("Unsupported database type or invalid connection string");
    }
    if (!currentDB->connect(connStr)) {
        currentDB.reset();
        throw std::runtime_error("Failed to connect to database");
    }
    return Value(true);  // or a Dex DB object wrapper if you prefer
}

// Dex function: Database.execute(query)
Value dex_database_execute(Interpreter& interp, const std::vector<Value>& args) {
    if (!currentDB) throw std::runtime_error("No database connected");
    if (args.size() != 1 || !args[0].isString())
        throw std::runtime_error("Database.execute expects 1 string argument");
    std::string query = args[0].asString();

    bool success = currentDB->execute(query);
    return Value(success);
}

// Dex function: Database.query(query)
Value dex_database_query(Interpreter& interp, const std::vector<Value>& args) {
    if (!currentDB) throw std::runtime_error("No database connected");
    if (args.size() != 1 || !args[0].isString())
        throw std::runtime_error("Database.query expects 1 string argument");
    std::string query = args[0].asString();

    auto results = currentDB->query(query);

    // Convert C++ vector<vector<string>> to Dex array of arrays (pseudo-code)
    Value dexResults = interp.createArray();
    for (auto& row : results) {
        Value dexRow = interp.createArray();
        for (auto& col : row) {
            dexRow.array_push(interp.createString(col));
        }
        dexResults.array_push(dexRow);
    }
    return dexResults;
}

// Register these functions under a Dex 'Database' global object or namespace

void registerDatabaseBindings(Interpreter& interp) {
    interp.registerFunction("Database.connect", dex_database_connect);
    interp.registerFunction("Database.execute", dex_database_execute);
    interp.registerFunction("Database.query", dex_database_query);
}
