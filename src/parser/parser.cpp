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
        std::cout << "DEBUG: parseProgram loop - Current token: " << current.toString() << std::endl;
        statements.push_back(parseStatement());
    }
    return statements;
}

StmtPtr Parser::parseStatement() {
    std::cout << "DEBUG: parseStatement starts. Current token: " << current.toString() << std::endl;
    // Skip leading newlines before parsing a statement
    while (check(TokenType::NEWLINE)) {
        std::cout << "DEBUG: parseStatement skipping NEWLINE. Current token: " << current.toString() << std::endl;
        advance();
    }
    std::cout << "DEBUG: parseStatement after newline skip. Current token: " << current.toString() << std::endl;

    if (check(TokenType::KEYWORD)) {
        // We need to consume the keyword here before checking its value
        // or ensure the `match` function correctly updates `current`
        // before the `if (current.value == ...)` checks.
        // Let's use `match` and then check `current.value` from the previous token.
        // Or, more simply, just check `current.value` without consuming yet.
        if (current.value == "if") {
            advance(); // Consume "if"
            return parseIfStatement();
        }
        if (current.value == "while") {
            advance(); // Consume "while"
            return parseWhileStatement();
        }
        if (current.value == "return") {
            advance(); // Consume "return"
            return parseReturnStatement();
        }
    }

    // If it starts with an identifier, it could be an assignment or an expression statement.
    // This function handles the lookahead and branching.
    if (check(TokenType::IDENTIFIER)) {
        return parseAssignmentOrExprStatement();
    }

    // Fallback: If none of the above, it must be a general expression statement
    // (e.g., a literal, a function call not starting with an identifier, etc.)
    ExprPtr expr = parseExpression();
    // Semicolon optional
    if ((check(TokenType::SYMBOL) && current.value == ";") || check(TokenType::NEWLINE)) {
        advance();
    }
    return std::make_shared<ExprStmt>(expr);
}

StmtPtr Parser::parseIfStatement() {
    // "if" already consumed by parseStatement
    consume(TokenType::SYMBOL, "Expected '(' after 'if'"); // '('
    ExprPtr condition = parseExpression();
    consume(TokenType::SYMBOL, "Expected ')' after condition"); // ')'

    StmtPtr thenBranch;
    if (check(TokenType::SYMBOL) && current.value == "{") { // Check, not match, as parseBlock consumes '{'
        thenBranch = parseBlock();
    } else {
        thenBranch = parseStatement();
    }

    StmtPtr elseBranch = nullptr;
    // Optional else
    if (current.type == TokenType::KEYWORD && current.value == "else") { // Check, not match
        advance(); // Consume "else"
        if (check(TokenType::SYMBOL) && current.value == "{") { // Check, not match
            elseBranch = parseBlock();
        } else {
            elseBranch = parseStatement();
        }
    }

    return std::make_shared<IfStmt>(IfStmt{condition, thenBranch, elseBranch});
}

StmtPtr Parser::parseWhileStatement() {
    // "while" already consumed by parseStatement
    consume(TokenType::SYMBOL, "Expected '(' after 'while'"); // '('
    ExprPtr condition = parseExpression();
    consume(TokenType::SYMBOL, "Expected ')' after condition"); // ')'

    StmtPtr body;
    if (check(TokenType::SYMBOL) && current.value == "{") { // Check, not match
        body = parseBlock();
    } else {
        body = parseStatement();
    }

    return std::make_shared<WhileStmt>(WhileStmt{condition, body});
}

StmtPtr Parser::parseReturnStatement() {
    // "return" already consumed by parseStatement
    ExprPtr value = nullptr;
    // Only parse an expression if there's something before the end of the statement
    if (!((check(TokenType::SYMBOL) && current.value == ";") || check(TokenType::NEWLINE)) && !check(TokenType::END_OF_FILE)) {
        value = parseExpression();
    }
    // Consume the semicolon or newline that ends the return statement
    if ((check(TokenType::SYMBOL) && current.value == ";") || check(TokenType::NEWLINE)) {
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

// This function handles statements that start with an IDENTIFIER,
// which can be either an assignment or an expression statement (like a function call).
StmtPtr Parser::parseAssignmentOrExprStatement() {
    // current is an IDENTIFIER at this point (e.g., 'server' or 'message')
    Token identifierToken = current;
    advance(); // Consume the IDENTIFIER

    // Peek the next token to determine if it's an assignment
    if (check(TokenType::SYMBOL) && current.value == "=") {
        // It's an assignment: IDENTIFIER = EXPRESSION
        advance(); // Consume '='
        ExprPtr value = parseExpression(); // Parse the value expression
        // Semicolon optional
        if ((check(TokenType::SYMBOL) && current.value == ";") || check(TokenType::NEWLINE)) {
            advance();
        }
        return std::make_shared<AssignStmt>(identifierToken.value, value);
    } else {
        // It's an expression statement that started with an identifier
        // The identifier has already been consumed and is in identifierToken.
        // We need to build the expression starting from this identifier.
        ExprPtr expr = std::make_shared<VariableExpr>(identifierToken.value);

        // Now continue parsing any member access or function calls on this expression
        while (check(TokenType::SYMBOL) && (current.value == "." || current.value == "(")) {
            if (current.value == ".") {
                advance(); // Consume '.'
                consume(TokenType::IDENTIFIER, "Expected identifier after '.' for member access");
                std::string propertyName = current.value;
                advance(); // Consume property name
                expr = std::make_shared<MemberAccessExpr>(expr, propertyName);
            } else if (current.value == "(") {
                expr = parseCall(expr); // Parse a function call on the current expression
            }
        }

        // Semicolon optional
        if ((check(TokenType::SYMBOL) && current.value == ";") || check(TokenType::NEWLINE)) {
            advance();
        }
        return std::make_shared<ExprStmt>(expr);
    }
}


ExprPtr Parser::parseExpression() {
    std::cout << "DEBUG: parseExpression starts. Current token: " << current.toString() << std::endl;
    return parseCallOrMemberAccess(); // This is the top-level expression parsing
}

// Handles expressions that can be chained with '.' for member access or '(' for function calls
ExprPtr Parser::parseCallOrMemberAccess() {
    std::cout << "DEBUG: parseCallOrMemberAccess starts. Current token: " << current.toString() << std::endl;
    ExprPtr expr = parsePrimary(); // Get the initial primary expression
    std::cout << "DEBUG: parseCallOrMemberAccess after parsePrimary. Current token: " << current.toString() << std::endl;

    while (check(TokenType::SYMBOL) && (current.value == "." || current.value == "(")) {
        if (current.value == ".") {
            advance(); // Consume '.'
            consume(TokenType::IDENTIFIER, "Expected identifier after '.' for member access");
            std::string propertyName = current.value;
            advance(); // Consume property name
            expr = std::make_shared<MemberAccessExpr>(expr, propertyName);
        } else if (current.value == "(") {
            expr = parseCall(expr); // Parse a function call on the current expression
        }
    }
    return expr;
}

ExprPtr Parser::parsePrimary() {
    std::cout << "DEBUG: parsePrimary starts. Current token: " << current.toString() << std::endl;
    if (check(TokenType::NUMBER) || check(TokenType::STRING)) {
        std::cout << "DEBUG: parsePrimary matched NUMBER/STRING." << std::endl;
        std::string val = current.value;
        advance();
        return std::make_shared<LiteralExpr>(val);
    }

    if (check(TokenType::IDENTIFIER)) {
        std::cout << "DEBUG: parsePrimary matched IDENTIFIER: " << current.value << std::endl;
        std::string name = current.value;
        advance(); // Consume the IDENTIFIER
        return std::make_shared<VariableExpr>(name);
    }

    if (match(TokenType::KEYWORD) && current.value == "func") {
        std::cout << "DEBUG: parsePrimary matched KEYWORD 'func'." << std::endl;
        consume(TokenType::SYMBOL, "Expected '(' after 'func'");
        consume(TokenType::SYMBOL, "Expected ')' after '(' in func");  // params empty for now

        StmtPtr body = parseBlock();
        auto funcExpr = std::make_shared<FuncExpr>();
        // Assuming FuncExpr's body is a vector of statements
        // This static_cast assumes parseBlock always returns a BlockStmt
        funcExpr->body = static_cast<BlockStmt*>(body.get())->statements;
        return funcExpr;
    }

    // Handle parentheses for grouping expressions
    if (match(TokenType::SYMBOL) && current.value == "(") {
        std::cout << "DEBUG: parsePrimary matched SYMBOL '(' for grouping." << std::endl;
        ExprPtr expr = parseExpression();
        consume(TokenType::SYMBOL, "Expected ')' after expression in parentheses");
        return expr;
    }

    std::cout << "DEBUG: parsePrimary throwing error. Current token: " << current.toString() << std::endl;
    throw std::runtime_error("Parser error: Unexpected token in expression: " + current.value);
}

// Parses a function call, assuming the `(` token is the current token upon entry.
ExprPtr Parser::parseCall(ExprPtr callee) {
    std::cout << "DEBUG: parseCall starts. Current token: " << current.toString() << std::endl;
    consume(TokenType::SYMBOL, "Expected '(' after function name or member"); // Consumes '('

    std::vector<ExprPtr> args;
    // Parse arguments until ')' or EOF
    while (current.type != TokenType::END_OF_FILE &&
           (current.type != TokenType::SYMBOL || current.value != ")")) {

        args.push_back(parseExpression()); // Parse an argument

        // If after parsing an argument, we find the closing parenthesis, break the loop
        if (current.type == TokenType::SYMBOL && current.value == ")") {
            break;
        }

        // If not a closing parenthesis, it must be a comma for more arguments
        if (current.type == TokenType::SYMBOL && current.value == ",") {
            advance(); // Consume the comma
        } else {
            // Unexpected token, it's neither ')' nor ','
            throw std::runtime_error("Parser error: Expected ',' or ')' in function call arguments, but found '" + current.value + "'");
        }
    }

    // After the loop, current token MUST be the closing parenthesis ')'
    consume(TokenType::SYMBOL, "Expected ')' after arguments"); // Consumes ')'

    return std::make_shared<CallExpr>(callee, args);
}

} // namespace dex
