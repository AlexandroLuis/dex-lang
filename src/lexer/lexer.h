#ifndef DEX_LEXER_H
#define DEX_LEXER_H

#include <string>
#include <vector>
#include <map> // For token types map

namespace dex {

enum class TokenType {
    IDENTIFIER,
    NUMBER,
    STRING,
    KEYWORD,
    SYMBOL, // +, -, *, /, =, ==, !=, <, >, <=, >=, (, ), {, }, [, ], ,, ., ;
    END_OF_FILE,
    NEWLINE, // For optional semicolons
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;

    std::string toString() const; // For debugging
};

class Lexer {
public:
    explicit Lexer(const std::string& source);
    Token getNextToken();
    Token peekNextToken();

private:
    std::string source;
    size_t position;
    int line;
    int column;
    Token next_token_buffer;
    bool has_peeked;

    char peekChar();
    char consumeChar();
    void skipWhitespace();
    void skipComments(); // <--- ADDED THIS DECLARATION
    Token readIdentifierOrKeyword();
    Token readNumber();
    Token readString();
    Token readSymbol();

    static std::map<std::string, TokenType> keywords;
    static std::map<char, TokenType> singleCharSymbols;
    static std::map<std::string, TokenType> multiCharSymbols;

    void initializeKeywords();
    void initializeSymbols();
};

} // namespace dex

#endif // DEX_LEXER_H
