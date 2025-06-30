#include "parser.h"
#include <stdexcept>
#include <iostream>

namespace dex {

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    advance();  // Initialize current token
}

void Parser::advance() {
    current = lexer.getNextToken();
}

bool Parser::check(TokenType type) {
    return current.type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& errMsg) {
    if (check(type)) {
        advance();
        return;
    }
    throw std::runtime_error("Parser error near token '" + current.value + "': " + errMsg);
}

std::vector<StmtPtr> Parser::parseProgram() {
    std::vector<StmtPtr> statements;
    while (!check(TokenType::END_OF_FILE)) {
        statements.push_back(parseStatement());
    }
    return statements;
}

StmtPtr Parser::parseStatement() {
    if (match(TokenType::KEYWORD)) {
        if (current.value == "if") {
            return parseIfStatement();
        }
        if (current.value == "while") {
            return parseWhileStatement();
        }
        if (current.value == "return") {
            return parseReturnStatement();
        }
    }

    if (check(TokenType::IDENTIFIER)) {
        return parseAssignmentOrExprStatement();
    }

    // Could add more statement types here

    // If none matched, parse as expression statement
    ExprPtr expr = parseExpression();
    // Semicolon optional
    if (match(TokenType::SEMICOLON) || check(TokenType::NEWLINE)) {
        advance();
    }
    return std::make_shared<ExprStmt>(expr);
}

StmtPtr Parser::parseIfStatement() {
    // Already matched "if"
    consume(TokenType::SYMBOL, "Expected '(' after 'if'"); // '('
    ExprPtr condition = parseExpression();
    consume(TokenType::SYMBOL, "Expected ')' after condition"); // ')'

    StmtPtr thenBranch;
    if (match(TokenType::SYMBOL) && current.value == "{") {
        thenBranch = parseBlock();
    } else {
        thenBranch = parseStatement();
    }

    StmtPtr elseBranch = nullptr;
    // Optional else
    if (match(TokenType::KEYWORD) && current.value == "else") {
        if (match(TokenType::SYMBOL) && current.value == "{") {
            elseBranch = parseBlock();
        } else {
            elseBranch = parseStatement();
        }
    }

    return std::make_shared<IfStmt>(IfStmt{condition, thenBranch, elseBranch});
}

StmtPtr Parser::parseWhileStatement() {
    // Already matched "while"
    consume(TokenType::SYMBOL, "Expected '(' after 'while'"); // '('
    ExprPtr condition = parseExpression();
    consume(TokenType::SYMBOL, "Expected ')' after condition"); // ')'

    StmtPtr body;
    if (match(TokenType::SYMBOL) && current.value == "{") {
        body = parseBlock();
    } else {
        body = parseStatement();
    }

    return std::make_shared<WhileStmt>(WhileStmt{condition, body});
}

StmtPtr Parser::parseReturnStatement() {
    ExprPtr value = nullptr;
    if (!check(TokenType::SEMICOLON) && !check(TokenType::NEWLINE)) {
        value = parseExpression();
    }
    if (match(TokenType::SEMICOLON) || check(TokenType::NEWLINE)) {
        advance();
    }
    return std::make_shared<ReturnStmt>(value);
}

StmtPtr Parser::parseBlock() {
    std::vector<StmtPtr> statements;
    consume(TokenType::SYMBOL, "Expected '{' to start block"); // '{'

    while (!check(TokenType::SYMBOL) || current.value != "}") {
        if (check(TokenType::END_OF_FILE)) {
            throw std::runtime_error("Parser error: Unexpected EOF in block");
        }
        statements.push_back(parseStatement());
    }

    consume(TokenType::SYMBOL, "Expected '}' to end block"); // '}'

    return std::make_shared<BlockStmt>(statements);
}

StmtPtr Parser::parseAssignmentOrExprStatement() {
    // Assume current token is IDENTIFIER
    std::string varName = current.value;
    advance();

    if (match(TokenType::SYMBOL) && current.value == "=") {
        ExprPtr value = parseExpression();
        if (match(TokenType::SEMICOLON) || check(TokenType::NEWLINE)) {
            advance();
        }
        return std::make_shared<AssignStmt>(varName, value);
    } else {
        // It's a function call or variable expr statement
        ExprPtr expr = std::make_shared<VariableExpr>(varName);

        // If next is '(', parse call expression
        if (check(TokenType::SYMBOL) && current.value == "(") {
            expr = parseCall(expr);
        }

        if (match(TokenType::SEMICOLON) || check(TokenType::NEWLINE)) {
            advance();
        }

        return std::make_shared<ExprStmt>(expr);
    }
}

ExprPtr Parser::parseExpression() {
    return parsePrimary();
}

ExprPtr Parser::parsePrimary() {
    if (check(TokenType::NUMBER) || check(TokenType::STRING)) {
        std::string val = current.value;
        advance();
        return std::make_shared<LiteralExpr>(val);
    }

    if (check(TokenType::IDENTIFIER)) {
        std::string name = current.value;
        advance();
        ExprPtr expr = std::make_shared<VariableExpr>(name);

        if (check(TokenType::SYMBOL) && current.value == "(") {
            expr = parseCall(expr);
        }
        return expr;
    }

    if (match(TokenType::KEYWORD) && current.value == "func") {
        consume(TokenType::SYMBOL, "Expected '(' after 'func'");
        consume(TokenType::SYMBOL, "Expected ')' after '(' in func");  // params empty for now

        StmtPtr body = parseBlock();
        auto funcExpr = std::make_shared<FuncExpr>();
        funcExpr->body = static_cast<BlockStmt*>(body.get())->statements;
        return funcExpr;
    }

    throw std::runtime_error("Parser error: Unexpected token in expression: " + current.value);
}

ExprPtr Parser::parseCall(ExprPtr callee) {
    consume(TokenType::SYMBOL, "Expected '(' after function name");

    std::vector<ExprPtr> args;
    if (!check(TokenType::SYMBOL) || current.value != ")") {
        do {
            args.push_back(parseExpression());
        } while (match(TokenType::SYMBOL) && current.value == ",");
    }
    consume(TokenType::SYMBOL, "Expected ')' after arguments");

    return std::make_shared<CallExpr>(callee, args);
}

}  // namespace dex
