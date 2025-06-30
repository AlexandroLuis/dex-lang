#include "fileio.h"
#include "json_utils.h"
#include "csv_utils.h"
#include "interpreter/interpreter.h"

namespace dex {

Value dex_readFile(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isString())
        throw std::runtime_error("readFile expects 1 string argument");
    return Value(readFile(args[0].asString()));
}

Value dex_writeFile(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 2 || !args[0].isString() || !args[1].isString())
        throw std::runtime_error("writeFile expects 2 string arguments");
    writeFile(args[0].asString(), args[1].asString());
    return Value::nil();
}

Value dex_parseJSON(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isString())
        throw std::runtime_error("parseJSON expects 1 string argument");

    auto j = parseJSON(args[0].asString());
    // Convert nlohmann::json to Dex Value here (you'll implement conversion)
    return interp.jsonToDexValue(j);
}

Value dex_toJSON(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1)
        throw std::runtime_error("toJSON expects 1 argument");

    // Convert Dex Value to nlohmann::json here (you'll implement conversion)
    nlohmann::json j = interp.dexValueToJson(args[0]);
    return Value(toJSON(j));
}

Value dex_parseCSV(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isString())
        throw std::runtime_error("parseCSV expects 1 string argument");

    auto rows = parseCSV(args[0].asString());
    // Convert vector<vector<string>> to Dex array of arrays
    return interp.csvToDexValue(rows);
}

Value dex_toCSV(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isArray())
        throw std::runtime_error("toCSV expects 1 array argument");

    // Convert Dex array of arrays to vector<vector<string>>
    auto rows = interp.dexValueToCSV(args[0]);
    return Value(toCSV(rows));
}

void registerFileIOBindings(Interpreter& interp) {
    interp.registerFunction("readFile", dex_readFile);
    interp.registerFunction("writeFile", dex_writeFile);
    interp.registerFunction("parseJSON", dex_parseJSON);
    interp.registerFunction("toJSON", dex_toJSON);
    interp.registerFunction("parseCSV", dex_parseCSV);
    interp.registerFunction("toCSV", dex_toCSV);
}

}
