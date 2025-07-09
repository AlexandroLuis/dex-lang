// src/runtime/env_binding.cpp

#include <cstdlib>
#include "../interpreter/interpreter.h" // Your interpreter headers, adapt as needed
#include <stdexcept>
#include <string> // Ensure std::string is included

namespace dex {

// The return type of this function must match the NativeFunction alias in interpreter.h,
// which is currently std::string.
std::string dex_getEnv(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isString()) {
        // In a real application, consider returning an error string
        // instead of throwing an exception that might not be caught
        // gracefully by the interpreter's call mechanism.
        std::cerr << "Runtime Error: getEnv expects exactly one string argument." << std::endl;
        return "Error: Invalid arguments for getEnv.";
    }

    const char* val = std::getenv(args[0].asString().c_str());
    if (!val) {
        return ""; // Return empty string if env var not set
    }
    // Convert the result to std::string as required by NativeFunction
    return std::string(val);
}

void registerEnvBindings(Interpreter& interp) {
    // Register the C++ function with the interpreter
    interp.registerFunction("getEnv", dex_getEnv);
}

} // namespace dex
