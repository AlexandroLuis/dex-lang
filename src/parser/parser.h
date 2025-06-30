// src/parser/parser.h
#ifndef DEX_PARSER_H
#define DEX_PARSER_H

#include "../lexer/lexer.h"
#include "ast.h"
#include <vector>
#include <memory>

namespace dex {

class Parser {
public:
    explicit Parser(Lexer& lexer);

    std::vector<StmtPtr> parseProgram();

private:
    Lexer& lexer;
    Token current;

    void advance();
    bool match(TokenType type);
    bool check(TokenType type);
    void consume(TokenType type, const std::string& errMsg);

    StmtPtr parseStatement();
    StmtPtr parseIfStatement();
    StmtPtr parseWhileStatement();
    StmtPtr parseReturnStatement();
    StmtPtr parseBlock();
    StmtPtr parseAssignmentOrExprStatement();

    ExprPtr parseExpression();
    ExprPtr parsePrimary();
    ExprPtr parseCall(ExprPtr callee);

    // Add helper parsing functions as needed
};

}  // namespace dex

#endif  // DEX_PARSER_H
