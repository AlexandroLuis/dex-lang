#ifndef DEX_INTERPRETER_H
#define DEX_INTERPRETER_H

#include "../parser/ast.h" // Assuming this defines StmtPtr, ExprPtr, AssignStmt, ExprStmt, ReturnStmt
#include <unordered_map>
#include <string>
#include <vector>
#include <functional> // For std::function
#include <variant>    // For a more robust Value type
#include <iostream>   // For basic output in Value for debugging/demonstration

// Include nlohmann/json for JSON-related types used in Interpreter methods
#include "../../external/nlohmann/json.hpp"

namespace dex {

// Forward declaration of Interpreter to be used in Value and function types
class Interpreter;

// A simple Value class to represent values in the Dex language.
// This is now extended to support strings, null, arrays, and objects.
class Value {
public:
    // Using std::variant for a more robust type system.
    // Added std::vector<Value> for arrays and std::unordered_map<std::string, Value> for objects.
    using ValueType = std::variant<std::string, std::nullptr_t, std::vector<Value>, std::unordered_map<std::string, Value>>;

    Value() : data(nullptr) {} // Default constructor for null value
    Value(const std::string& s) : data(s) {}
    Value(const char* s) : data(std::string(s)) {}
    Value(std::nullptr_t) : data(nullptr) {}
    Value(const std::vector<Value>& arr) : data(arr) {}
    Value(const std::unordered_map<std::string, Value>& obj) : data(obj) {}

    // Static method to create a null Value
    static Value nil() {
        return Value(nullptr);
    }

    // Type checking methods
    bool isString() const {
        return std::holds_alternative<std::string>(data);
    }
    bool isNull() const {
        return std::holds_alternative<std::nullptr_t>(data);
    }
    bool isArray() const {
        return std::holds_alternative<std::vector<Value>>(data);
    }
    bool isObject() const {
        return std::holds_alternative<std::unordered_map<std::string, Value>>(data);
    }

    // Get value methods. Throws if not the correct type.
    const std::string& asString() const {
        return std::get<std::string>(data);
    }
    const std::vector<Value>& asArray() const {
        return std::get<std::vector<Value>>(data);
    }
    const std::unordered_map<std::string, Value>& asObject() const {
        return std::get<std::unordered_map<std::string, Value>>(data);
    }

    // For debugging/printing (simple version)
    std::string toString() const {
        if (isString()) {
            return asString();
        } else if (isNull()) {
            return "null";
        } else if (isArray()) {
            std::string s = "[";
            const auto& arr = asArray();
            for (size_t i = 0; i < arr.size(); ++i) {
                s += arr[i].toString();
                if (i < arr.size() - 1) s += ", ";
            }
            s += "]";
            return s;
        } else if (isObject()) {
            std::string s = "{";
            const auto& obj = asObject();
            bool first = true;
            for (const auto& pair : obj) {
                if (!first) s += ", ";
                s += "\"" + pair.first + "\": " + pair.second.toString();
                first = false;
            }
            s += "}";
            return s;
        }
        return "[Unknown Value Type]";
    }

private:
    ValueType data;
};


// Type alias for native C++ functions callable from Dex
// Returns dex::Value to match fileio_binding.cpp's intent
using NativeFunction = std::function<Value(Interpreter&, const std::vector<Value>&)>;

class Interpreter {
public:
    Interpreter() = default;

    void interpret(const std::vector<StmtPtr>& statements);

    // Method to register native C++ functions
    void registerFunction(const std::string& name, NativeFunction func) {
        nativeFunctions[name] = func;
    }

    // Method to call a registered native function (used internally by interpreter)
    // Returns dex::Value
    Value callNativeFunction(const std::string& name, const std::vector<Value>& args) {
        if (nativeFunctions.count(name)) {
            return nativeFunctions[name](*this, args);
        }
        std::cerr << "Runtime Error: Native function '" << name << "' not found." << std::endl;
        return Value::nil(); // Return a null value or throw an exception
    }

    // Placeholder conversion methods for JSON/CSV (to be implemented fully)
    // These methods convert between nlohmann::json/std::vector<std::vector<std::string>>
    // and the Dex Value type. Their full implementation depends on how you map
    // JSON/CSV types to your Dex Value's variant types.
    Value jsonToDexValue(const nlohmann::json& j) {
        // Placeholder: Implement conversion from nlohmann::json to dex::Value
        // This will involve recursively converting JSON objects/arrays/primitives
        // to dex::Value's internal variant types.
        std::cerr << "Warning: jsonToDexValue is a placeholder. Implement full conversion." << std::endl;
        if (j.is_string()) return Value(j.get<std::string>());
        if (j.is_null()) return Value::nil();
        if (j.is_boolean()) return Value(j.get<bool>() ? "true" : "false"); // Convert bool to string for now
        if (j.is_number()) return Value(std::to_string(j.get<double>())); // Convert number to string for now
        if (j.is_array()) {
            std::vector<Value> arr_val;
            for (const auto& el : j) {
                arr_val.push_back(jsonToDexValue(el));
            }
            return Value(arr_val);
        }
        if (j.is_object()) {
            std::unordered_map<std::string, Value> obj_val;
            for (nlohmann::json::const_iterator it = j.begin(); it != j.end(); ++it) {
                obj_val[it.key()] = jsonToDexValue(it.value());
            }
            return Value(obj_val);
        }
        return Value::nil(); // Default for unsupported types
    }

    nlohmann::json dexValueToJson(const Value& val) {
        // Placeholder: Implement conversion from dex::Value to nlohmann::json
        // This will involve recursively converting dex::Value's internal variant types
        // to nlohmann::json objects/arrays/primitives.
        std::cerr << "Warning: dexValueToJson is a placeholder. Implement full conversion." << std::endl;
        if (val.isString()) return nlohmann::json(val.asString());
        if (val.isNull()) return nlohmann::json(); // nlohmann::json() creates null
        if (val.isArray()) {
            nlohmann::json j_arr = nlohmann::json::array();
            for (const auto& el_val : val.asArray()) {
                j_arr.push_back(dexValueToJson(el_val));
            }
            return j_arr;
        }
        if (val.isObject()) {
            nlohmann::json j_obj = nlohmann::json::object();
            for (const auto& pair : val.asObject()) {
                j_obj[pair.first] = dexValueToJson(pair.second);
            }
            return j_obj;
        }
        return nlohmann::json(); // Default for unsupported types
    }

    Value csvToDexValue(const std::vector<std::vector<std::string>>& rows) {
        // Placeholder: Implement conversion from CSV rows to dex::Value (e.g., an array of arrays of strings)
        std::cerr << "Warning: csvToDexValue is a placeholder. Implement full conversion." << std::endl;
        std::vector<Value> dex_rows;
        for (const auto& row : rows) {
            std::vector<Value> dex_cells;
            for (const auto& cell : row) {
                dex_cells.push_back(Value(cell));
            }
            dex_rows.push_back(Value(dex_cells));
        }
        return Value(dex_rows);
    }

    std::vector<std::vector<std::string>> dexValueToCSV(const Value& val) {
        // Placeholder: Implement conversion from dex::Value (assuming it's an array of arrays of strings) to CSV rows
        std::cerr << "Warning: dexValueToCSV is a placeholder. Implement full conversion." << std::endl;
        std::vector<std::vector<std::string>> rows;
        if (val.isArray()) {
            for (const auto& row_val : val.asArray()) {
                if (row_val.isArray()) {
                    std::vector<std::string> row_str;
                    for (const auto& cell_val : row_val.asArray()) {
                        if (cell_val.isString()) {
                            row_str.push_back(cell_val.asString());
                        } else {
                            row_str.push_back(cell_val.toString()); // Convert non-strings to string
                        }
                    }
                    rows.push_back(row_str);
                }
            }
        }
        return rows;
    }


private:
    std::unordered_map<std::string, std::string> variables; // Simple string vars for now
    std::unordered_map<std::string, NativeFunction> nativeFunctions; // Registered native functions

    void execute(StmtPtr stmt);
    void executeBlock(const std::vector<StmtPtr>& statements);
    std::string evaluate(ExprPtr expr); // THIS REMAINS std::string

    void executeAssign(std::shared_ptr<AssignStmt> stmt);
    void executeExprStmt(std::shared_ptr<ExprStmt> stmt);
    void executeReturn(std::shared_ptr<ReturnStmt> stmt);

    // Add more execute methods as you expand AST and runtime
};

} // namespace dex

#endif // DEX_INTERPRETER_H
