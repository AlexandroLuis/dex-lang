#include "../src/lexer/lexer.h"
#include <iostream>

int main() {
    std::string source = "func test() { return 123; }";
    dex::Lexer lexer(source);
    dex::Token token;
    do {
        token = lexer.getNextToken();
        std::cout << "Token: " << static_cast<int>(token.type) << " value: " << token.value << "\n";
    } while (token.type != dex::TokenType::END_OF_FILE);
    return 0;
}
