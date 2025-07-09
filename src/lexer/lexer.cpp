#include "lexer.h"
#include <stdexcept>
#include <cctype> // For isalpha, isdigit, isalnum

namespace dex {

// Initialize static maps
std::map<std::string, TokenType> Lexer::keywords;
std::map<char, TokenType> Lexer::singleCharSymbols;
std::map<std::string, TokenType> Lexer::multiCharSymbols;

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1), has_peeked(false) {
    initializeKeywords();
    initializeSymbols();
}

void Lexer::initializeKeywords() {
    keywords["if"] = TokenType::KEYWORD;
    keywords["else"] = TokenType::KEYWORD;
    keywords["while"] = TokenType::KEYWORD;
    keywords["return"] = TokenType::KEYWORD;
    keywords["func"] = TokenType::KEYWORD; // For anonymous functions
    // Add other keywords as needed
}

void Lexer::initializeSymbols() {
    singleCharSymbols['+'] = TokenType::SYMBOL;
    singleCharSymbols['-'] = TokenType::SYMBOL;
    singleCharSymbols['*'] = TokenType::SYMBOL;
    singleCharSymbols['/'] = TokenType::SYMBOL; // Keep '/' as a symbol for division
    singleCharSymbols['='] = TokenType::SYMBOL;
    singleCharSymbols['('] = TokenType::SYMBOL;
    singleCharSymbols[')'] = TokenType::SYMBOL;
    singleCharSymbols['{'] = TokenType::SYMBOL;
    singleCharSymbols['}'] = TokenType::SYMBOL;
    singleCharSymbols['['] = TokenType::SYMBOL;
    singleCharSymbols[']'] = TokenType::SYMBOL;
    singleCharSymbols[','] = TokenType::SYMBOL;
    singleCharSymbols[';'] = TokenType::SYMBOL;
    singleCharSymbols['.'] = TokenType::SYMBOL; // Add dot for member access

    multiCharSymbols["=="] = TokenType::SYMBOL;
    multiCharSymbols["!="] = TokenType::SYMBOL;
    multiCharSymbols["<="] = TokenType::SYMBOL;
    multiCharSymbols[">="] = TokenType::SYMBOL;
    multiCharSymbols["<"] = TokenType::SYMBOL;
    multiCharSymbols[">"] = TokenType::SYMBOL;
    // Add other multi-character symbols
}

char Lexer::peekChar() {
    if (position >= source.length()) {
        return '\0';
    }
    return source[position];
}

char Lexer::consumeChar() {
    if (position >= source.length()) {
        return '\0';
    }
    char c = source[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (position < source.length() && std::isspace(peekChar()) && peekChar() != '\n') {
        consumeChar();
    }
}

void Lexer::skipComments() {
    while (true) {
        // Check for single-line comment "//"
        if (peekChar() == '/' && position + 1 < source.length() && source[position + 1] == '/') {
            consumeChar(); // Consume first '/'
            consumeChar(); // Consume second '/'
            // Consume characters until newline or EOF
            while (position < source.length() && peekChar() != '\n') {
                consumeChar();
            }
            // Do not consume newline here; let getNextToken handle it if it's the next token.
            // This ensures NEWLINE tokens are still generated.
        }
        // Check for multi-line comment "/* ... */"
        else if (peekChar() == '/' && position + 1 < source.length() && source[position + 1] == '*') {
            consumeChar(); // Consume '/'
            consumeChar(); // Consume '*'
            // Consume characters until "*/" or EOF
            while (position < source.length() && !(peekChar() == '*' && position + 1 < source.length() && source[position + 1] == '/')) {
                consumeChar();
            }
            if (position + 1 < source.length() && peekChar() == '*' && source[position + 1] == '/') {
                consumeChar(); // Consume '*'
                consumeChar(); // Consume '/'
            } else {
                // Unclosed multi-line comment at EOF
                throw std::runtime_error("Lexer error: Unclosed multi-line comment at line " + std::to_string(line) + ", column " + std::to_string(column));
            }
        } else {
            // No more comments found
            break;
        }
        // After skipping a comment, there might be more whitespace or another comment
        skipWhitespace();
    }
}


Token Lexer::readIdentifierOrKeyword() {
    std::string value;
    int startColumn = column;
    while (position < source.length() && (std::isalnum(peekChar()) || peekChar() == '_')) {
        value += consumeChar();
    }

    if (keywords.count(value)) {
        return {keywords[value], value, line, startColumn};
    }
    return {TokenType::IDENTIFIER, value, line, startColumn};
}

Token Lexer::readNumber() {
    std::string value;
    int startColumn = column;
    while (position < source.length() && std::isdigit(peekChar())) {
        value += consumeChar();
    }
    if (peekChar() == '.') {
        value += consumeChar(); // Consume '.'
        while (position < source.length() && std::isdigit(peekChar())) {
            value += consumeChar();
        }
    }
    return {TokenType::NUMBER, value, line, startColumn};
}

Token Lexer::readString() {
    std::string value;
    int startColumn = column;
    consumeChar(); // Consume opening quote '"'
    while (position < source.length() && peekChar() != '"') {
        if (peekChar() == '\\') { // Handle escape sequences
            value += consumeChar(); // Consume '\'
            if (position < source.length()) {
                value += consumeChar(); // Consume escaped char
            }
        } else {
            value += consumeChar();
        }
    }
    consumeChar(); // Consume closing quote '"'
    return {TokenType::STRING, value, line, startColumn};
}

Token Lexer::readSymbol() {
    int startColumn = column;
    std::string twoCharSymbol = "";
    if (position + 1 < source.length()) {
        twoCharSymbol += peekChar();
        twoCharSymbol += source[position + 1];
    }

    if (multiCharSymbols.count(twoCharSymbol)) {
        consumeChar();
        consumeChar();
        return {multiCharSymbols[twoCharSymbol], twoCharSymbol, line, startColumn};
    }

    char singleChar = consumeChar();
    if (singleCharSymbols.count(singleChar)) {
        return {singleCharSymbols[singleChar], std::string(1, singleChar), line, startColumn};
    }
    throw std::runtime_error("Lexer error: Unknown symbol '" + std::string(1, singleChar) + "' at line " + std::to_string(line) + ", column " + std::to_string(startColumn));
}

Token Lexer::getNextToken() {
    if (has_peeked) {
        has_peeked = false;
        return next_token_buffer;
    }

    // Loop to handle multiple comments/whitespace blocks
    while (true) {
        skipWhitespace(); // Skip spaces and tabs
        skipComments();   // Skip comments
        // After skipping, check again if there's more whitespace or comments
        // This handles cases like `// comment\n   // another comment`
        if (!std::isspace(peekChar()) && !(peekChar() == '/' && (source[position+1] == '/' || source[position+1] == '*'))) {
            break; // No more whitespace or comments, proceed to tokenize
        }
        // If we found more whitespace or comments, the loop continues
    }


    if (position >= source.length()) {
        return {TokenType::END_OF_FILE, "", line, column};
    }

    char c = peekChar();

    if (c == '\n') {
        consumeChar(); // Consume the newline
        return {TokenType::NEWLINE, "\n", line - 1, column}; // Report original line
    }

    if (std::isalpha(c) || c == '_') {
        return readIdentifierOrKeyword();
    }
    if (std::isdigit(c)) {
        return readNumber();
    }
    if (c == '"') {
        return readString();
    }
    // Check for symbols, including potential start of comments (which should have been skipped)
    if (singleCharSymbols.count(c) || (position + 1 < source.length() && multiCharSymbols.count(std::string(1, c) + source[position+1]))) {
        return readSymbol();
    }

    throw std::runtime_error("Lexer error: Unexpected character '" + std::string(1, c) + "' at line " + std::to_string(line) + ", column " + std::to_string(column));
}

Token Lexer::peekNextToken() {
    if (!has_peeked) {
        // Store current position and line/column
        size_t original_position = position;
        int original_line = line;
        int original_column = column;

        next_token_buffer = getNextToken(); // Get the next token normally
        has_peeked = true;

        // Restore position and line/column
        position = original_position;
        line = original_line;
        column = original_column;
    }
    return next_token_buffer;
}

std::string Token::toString() const {
    std::string type_str;
    switch (type) {
        case TokenType::IDENTIFIER: type_str = "IDENTIFIER"; break;
        case TokenType::NUMBER: type_str = "NUMBER"; break;
        case TokenType::STRING: type_str = "STRING"; break;
        case TokenType::KEYWORD: type_str = "KEYWORD"; break;
        case TokenType::SYMBOL: type_str = "SYMBOL"; break;
        case TokenType::END_OF_FILE: type_str = "END_OF_FILE"; break;
        case TokenType::NEWLINE: type_str = "NEWLINE"; break;
        case TokenType::UNKNOWN: type_str = "UNKNOWN"; break;
    }
    return "Token(Type: " + type_str + ", Value: '" + value + "', Line: " + std::to_string(line) + ", Column: " + std::to_string(column) + ")";
}

} // namespace dex
