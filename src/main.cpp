#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "interpreter/interpreter.h"
#include "dotenv.h"

// Forward declarations of your binding registration functions
namespace dex {
    void registerEnvBindings(Interpreter&);
    void registerDatabaseBindings(Interpreter&);

    // Add this:
    void registerFileIOBindings(Interpreter&);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: dex run <source.d>\n";
        return 1;
    }

    if (std::filesystem::exists(".env")) {
        dotenv::env.load_dotenv(".env");
        std::cout << "[INFO] Loaded .env file\n";
    } else {
        std::cout << "[INFO] No .env file found, skipping\n";
    }

    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Error: Could not open file " << argv[1] << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        dex::Lexer lexer(source);
        dex::Parser parser(lexer);
        auto program = parser.parseProgram();

        dex::Interpreter interpreter;

        dex::registerEnvBindings(interpreter);
        dex::registerDatabaseBindings(interpreter);

        // Register your new File IO / JSON / CSV bindings here:
        dex::registerFileIOBindings(interpreter);

        interpreter.interpret(program);

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
