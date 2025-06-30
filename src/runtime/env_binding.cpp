// src/runtime/env_binding.cpp

#include <cstdlib>
#include "interpreter.h"  // Your interpreter headers, adapt as needed
#include <stdexcept>

namespace dex {

Value dex_getEnv(Interpreter& interp, const std::vector<Value>& args) {
    if (args.size() != 1 || !args[0].isString()) {
        throw std::runtime_error("getEnv expects exactly one string argument");
    }

    const char* val = std::getenv(args[0].asString().c_str());
    if (!val) {
        return Value("");  // Return empty string if env var not set
    }
    return Value(std::string(val));
}

void registerEnvBindings(Interpreter& interp) {
    interp.registerFunction("getEnv", dex_getEnv);
}

} // namespace dex
