#ifndef DEX_LEXER_H
#define DEX_LEXER_H

#include <string>
#include <unordered_map>

namespace dex {

enum class TokenType {
    IDENTIFIER,
    KEYWORD,
    STRING,
    NUMBER,
    SYMBOL,
    SEMICOLON,
    NEWLINE,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    explicit Lexer(const std::string& source);

    Token getNextToken();

private:
    std::string src;
    size_t pos;
    size_t length;

    char peek() const;
    char advance();
    void skipWhitespace();
    void skipComment();
    bool isAtEnd() const;
    Token string();
    Token number();
    Token identifier();

    static const std::unordered_map<std::string, TokenType> keywords;
};

} // namespace dex

#endif // DEX_LEXER_H
