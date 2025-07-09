#include "fileio.h"      // Assumed to provide readFile, writeFile
#include "../../external/nlohmann/json_utils.h"  // Assumed to provide parseJSON, toJSON (for nlohmann::json)
#include "csv_utils.h"   // Assumed to provide parseCSV, toCSV (for vector<vector<string>>)
#include "../interpreter/interpreter.h" // Your updated interpreter.h
#include <stdexcept>     // For std::runtime_error
#include <iostream>      // For std::cerr (for error messages)

// Assuming nlohmann/json.hpp is included by json_utils.h or directly in your build system.
// If not, you might need to add: #include "nlohmann/json.hpp" here.

namespace dex {

Value dex_readFile(Interpreter& interp, const std::vector<Value>& args) {
    (void)interp; // Suppress unused parameter warning

    if (args.size() != 1 || !args[0].isString()) {
        std::cerr << "Runtime Error: readFile expects 1 string argument." << std::endl;
        throw std::runtime_error("readFile expects 1 string argument");
    }
    // Assuming readFile is a global or utility function that returns std::string
    return Value(readFile(args[0].asString()));
}

Value dex_writeFile(Interpreter& interp, const std::vector<Value>& args) {
    (void)interp; // Suppress unused parameter warning

    if (args.size() != 2 || !args[0].isString() || !args[1].isString()) {
        std::cerr << "Runtime Error: writeFile expects 2 string arguments." << std::endl;
        throw std::runtime_error("writeFile expects 2 string arguments");
    }
    // Assuming writeFile is a global or utility function
    writeFile(args[0].asString(), args[1].asString());
    return Value::nil();
}

Value dex_parseJSON(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isString()) {
        std::cerr << "Runtime Error: parseJSON expects 1 string argument." << std::endl;
        throw std::runtime_error("parseJSON expects 1 string argument");
    }

    // Assuming parseJSON is a global or utility function that returns nlohmann::json
    auto j = parseJSON(args[0].asString());
    // Convert nlohmann::json to Dex Value using the Interpreter's method
    return interp.jsonToDexValue(j);
}

Value dex_toJSON(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1) {
        std::cerr << "Runtime Error: toJSON expects 1 argument." << std::endl;
        throw std::runtime_error("toJSON expects 1 argument");
    }

    // Convert Dex Value to nlohmann::json using the Interpreter's method
    nlohmann::json j = interp.dexValueToJson(args[0]);
    // Assuming toJSON is a global or utility function that takes nlohmann::json and returns std::string
    return Value(toJSON(j));
}

Value dex_parseCSV(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isString()) {
        std::cerr << "Runtime Error: parseCSV expects 1 string argument." << std::endl;
        throw std::runtime_error("parseCSV expects 1 string argument");
    }

    // Assuming parseCSV is a global or utility function that returns std::vector<std::vector<std::string>>
    auto rows = parseCSV(args[0].asString());
    // Convert vector<vector<string>> to Dex array of arrays using the Interpreter's method
    return interp.csvToDexValue(rows);
}

Value dex_toCSV(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isArray()) {
        std::cerr << "Runtime Error: toCSV expects 1 array argument." << std::endl;
        throw std::runtime_error("toCSV expects 1 array argument");
    }

    // Convert Dex array of arrays to vector<vector<string>> using the Interpreter's method
    auto rows = interp.dexValueToCSV(args[0]);
    // Assuming toCSV is a global or utility function that takes vector<vector<string>> and returns std::string
    return Value(toCSV(rows));
}

void registerFileIOBindings(Interpreter& interp) {
    interp.registerFunction("FileIO.readFile", dex_readFile);
    interp.registerFunction("FileIO.writeFile", dex_writeFile);
    interp.registerFunction("FileIO.parseJSON", dex_parseJSON);
    interp.registerFunction("FileIO.toJSON", dex_toJSON);
    interp.registerFunction("FileIO.parseCSV", dex_parseCSV);
    interp.registerFunction("FileIO.toCSV", dex_toCSV);
}

} // namespace dex
