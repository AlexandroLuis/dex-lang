#include "lexer.h"
#include <cctype>

namespace dex {

// Define keywords here
const std::unordered_map<std::string, TokenType> Lexer::keywords = {
    {"func", TokenType::KEYWORD},
    {"return", TokenType::KEYWORD},
    {"if", TokenType::KEYWORD},
    {"while", TokenType::KEYWORD}
};

Lexer::Lexer(const std::string& source)
    : src(source), pos(0), length(source.length()) {}

char Lexer::peek() const {
    return pos < length ? src[pos] : '\0';
}

char Lexer::advance() {
    return pos < length ? src[pos++] : '\0';
}

bool Lexer::isAtEnd() const {
    return pos >= length;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(peek()) && peek() != '\n') {
        advance();
    }
}

void Lexer::skipComment() {
    if (peek() == '/') {
        if ((pos + 1) < length) {
            if (src[pos + 1] == '/') {
                // single line comment
                advance(); // consume first '/'
                advance(); // consume second '/'
                while (!isAtEnd() && peek() != '\n') advance();
            } else if (src[pos + 1] == '*') {
                // multi line comment
                advance(); // consume first '/'
                advance(); // consume '*'
                while (!isAtEnd()) {
                    if (peek() == '*' && (pos + 1) < length && src[pos + 1] == '/') {
                        advance();
                        advance();
                        break;
                    }
                    advance();
                }
            }
        }
    }
}

Token Lexer::string() {
    char quote = advance(); // Consume ' or "
    std::string value;

    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\\') {
            advance(); // consume \
            if (!isAtEnd()) {
                char esc = advance();
                switch (esc) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case '\\': value += '\\'; break;
                    case '\'': value += '\''; break;
                    case '\"': value += '\"'; break;
                    default: value += esc; break;
                }
            }
        } else {
            value += advance();
        }
    }
    advance(); // consume closing quote
    return {TokenType::STRING, value};
}

Token Lexer::number() {
    std::string value;
    bool hasDot = false;
    while (!isAtEnd() && (std::isdigit(peek()) || (!hasDot && peek() == '.'))) {
        if (peek() == '.') hasDot = true;
        value += advance();
    }
    return {TokenType::NUMBER, value};
}

Token Lexer::identifier() {
    std::string value;
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        value += advance();
    }

    auto it = keywords.find(value);
    if (it != keywords.end()) {
        return {it->second, value};
    }
    return {TokenType::IDENTIFIER, value};
}

Token Lexer::getNextToken() {
    while (!isAtEnd()) {
        skipWhitespace();
        skipComment();

        if (isAtEnd()) break;

        char c = peek();

        if (std::isalpha(c) || c == '_') return identifier();
        if (std::isdigit(c)) return number();
        if (c == '"' || c == '\'') return string();

        if (c == '=' || c == '(' || c == ')' || c == '{' || c == '}' || c == ',' ||
            c == '+' || c == '-' || c == '*' || c == '/') {
            advance();
            return {TokenType::SYMBOL, std::string(1, c)};
        }

        if (c == ';') {
            advance();
            return {TokenType::SEMICOLON, ";"};
        }

        if (c == '\n') {
            advance();
            return {TokenType::NEWLINE, "\\n"};
        }

        // Unknown character, skip it
        advance();
    }

    return {TokenType::END_OF_FILE, ""};
}

} // namespace dex
