#include "../interpreter/interpreter.h" // Include the updated interpreter.h
#include <sqlite3.h>     // SQLite library header
#include <iostream>      // For std::cerr, std::cout
#include <vector>        // For std::vector
#include <string>        // For std::string

// Global SQLite database connection handle
// In a real application, this should be managed more robustly,
// e.g., passed around or stored in the Interpreter state.
static sqlite3* db_connection = nullptr;

namespace dex {

// Helper function to convert SQLite error codes to a string
std::string sqliteErrorToString(int rc, sqlite3* db) {
    if (rc != SQLITE_OK) {
        std::string error_msg = "SQLite Error: " + std::string(sqlite3_errmsg(db));
        std::cerr << error_msg << std::endl;
        return error_msg;
    }
    return "OK";
}

/**
 * @brief Connects to an SQLite database.
 * Dex usage: `Database.connect("path/to/database.db")`
 * @param interp The interpreter instance.
 * @param args A vector of Dex Values. Expects one argument: database path (string).
 * @return A string indicating success ("OK") or an error message.
 */
std::string dex_database_connect(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isString()) {
        std::cerr << "Database.connect: Expected 1 string argument (database path)." << std::endl;
        return "Error: Invalid arguments for Database.connect";
    }

    const std::string& db_path = args[0].asString();

    // Close existing connection if any
    if (db_connection) {
        sqlite3_close(db_connection);
        db_connection = nullptr;
    }

    int rc = sqlite3_open(db_path.c_str(), &db_connection);
    if (rc) {
        std::string error_msg = "Can't open database: " + std::string(sqlite3_errmsg(db_connection));
        std::cerr << error_msg << std::endl;
        // Ensure db_connection is null if open fails, as sqlite3_open might not set it on failure
        if (db_connection) {
            sqlite3_close(db_connection);
            db_connection = nullptr;
        }
        return error_msg;
    } else {
        std::cout << "Successfully connected to database: " << db_path << std::endl;
        return "OK";
    }
}

/**
 * @brief Executes a non-query SQL statement (e.g., INSERT, UPDATE, DELETE, CREATE TABLE).
 * Dex usage: `Database.execute("CREATE TABLE users (id INT, name TEXT)")`
 * @param interp The interpreter instance.
 * @param args A vector of Dex Values. Expects one argument: SQL statement (string).
 * @return A string indicating success ("OK") or an error message.
 */
std::string dex_database_execute(Interpreter& interp, const std::vector<Value>& args) {
    if (!db_connection) {
        return "Error: Not connected to a database. Call Database.connect first.";
    }
    if (args.size() != 1 || !args[0].isString()) {
        std::cerr << "Database.execute: Expected 1 string argument (SQL statement)." << std::endl;
        return "Error: Invalid arguments for Database.execute";
    }

    const std::string& sql = args[0].asString();
    char* zErrMsg = 0;
    int rc = sqlite3_exec(db_connection, sql.c_str(), 0, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        std::string error_msg = "SQL error: " + std::string(zErrMsg);
        sqlite3_free(zErrMsg);
        std::cerr << error_msg << std::endl;
        return error_msg;
    } else {
        std::cout << "SQL executed successfully: " << sql << std::endl;
        return "OK";
    }
}

// Callback function for sqlite3_exec in dex_database_query
// This simple callback just prints results and appends to a string.
static int query_callback(void* data, int argc, char** argv, char** azColName) {
    std::string* result_str = static_cast<std::string*>(data);
    for (int i = 0; i < argc; i++) {
        result_str->append(azColName[i]);
        result_str->append(": ");
        result_str->append(argv[i] ? argv[i] : "NULL");
        if (i < argc - 1) {
            result_str->append(", ");
        }
    }
    result_str->append("\n"); // Newline for each row
    return 0;
}

/**
 * @brief Executes a SQL query (e.g., SELECT) and returns results.
 * Dex usage: `Database.query("SELECT * FROM users WHERE id = 1")`
 * @param interp The interpreter instance.
 * @param args A vector of Dex Values. Expects one argument: SQL query (string).
 * @return A string containing the query results as a string, or an error message.
 */
std::string dex_database_query(Interpreter& interp, const std::vector<Value>& args) {
    if (!db_connection) {
        return "Error: Not connected to a database. Call Database.connect first.";
    }
    if (args.size() != 1 || !args[0].isString()) {
        std::cerr << "Database.query: Expected 1 string argument (SQL query)." << std::endl;
        return "Error: Invalid arguments for Database.query";
    }

    const std::string& sql = args[0].asString();
    char* zErrMsg = 0;
    std::string query_result_str; // String to accumulate query results

    int rc = sqlite3_exec(db_connection, sql.c_str(), query_callback, &query_result_str, &zErrMsg);

    if (rc != SQLITE_OK) {
        std::string error_msg = "SQL error: " + std::string(zErrMsg);
        sqlite3_free(zErrMsg);
        std::cerr << error_msg << std::endl;
        return error_msg;
    } else {
        std::cout << "SQL query executed successfully: " << sql << std::endl;
        // If no rows returned, query_result_str might be empty.
        if (query_result_str.empty()) {
            return "Query successful, no rows returned.";
        }
        return query_result_str;
    }
}

/**
 * @brief Registers database binding functions with the Dex Interpreter.
 * @param interp The interpreter instance to register functions with.
 */
void registerDatabaseBindings(Interpreter& interp) {
    // Register the C++ functions as native functions callable from Dex
    interp.registerFunction("Database.connect", dex_database_connect);
    interp.registerFunction("Database.execute", dex_database_execute);
    interp.registerFunction("Database.query", dex_database_query);
}

} // namespace dex
