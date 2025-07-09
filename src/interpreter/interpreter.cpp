#include "interpreter.h"
#include <iostream>

namespace dex {

void Interpreter::interpret(const std::vector<StmtPtr>& statements) {
    for (auto& stmt : statements) {
        execute(stmt);
    }
}

void Interpreter::execute(StmtPtr stmt) {
    if (auto assign = std::dynamic_pointer_cast<AssignStmt>(stmt)) {
        executeAssign(assign);
    } else if (auto exprStmt = std::dynamic_pointer_cast<ExprStmt>(stmt)) {
        executeExprStmt(exprStmt);
    } else if (auto retStmt = std::dynamic_pointer_cast<ReturnStmt>(stmt)) {
        executeReturn(retStmt);
    } else if (auto block = std::dynamic_pointer_cast<BlockStmt>(stmt)) {
        executeBlock(block->statements);
    } else {
        std::cerr << "Unknown statement type in interpreter\n";
    }
}

void Interpreter::executeBlock(const std::vector<StmtPtr>& statements) {
    for (auto& stmt : statements) {
        execute(stmt);
    }
}

std::string Interpreter::evaluate(ExprPtr expr) {
    if (auto lit = std::dynamic_pointer_cast<LiteralExpr>(expr)) {
        return lit->value;
    } else if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        auto it = variables.find(var->name);
        if (it != variables.end()) {
            return it->second;
        } else {
            std::cerr << "Undefined variable: " << var->name << "\n";
            return "";
        }
    } else if (auto call = std::dynamic_pointer_cast<CallExpr>(expr)) {
        // For now, simple stub
        std::cout << "Function call: (not implemented)\n";
        return "";
    } else {
        std::cerr << "Unknown expression type\n";
        return "";
    }
}

void Interpreter::executeAssign(std::shared_ptr<AssignStmt> stmt) {
    std::string value = evaluate(stmt->value);
    variables[stmt->name] = value;
}

void Interpreter::executeExprStmt(std::shared_ptr<ExprStmt> stmt) {
    evaluate(stmt->expression);
}

void Interpreter::executeReturn(std::shared_ptr<ReturnStmt> stmt) {
    if (stmt->value) {
        std::cout << "Return: " << evaluate(stmt->value) << "\n";
    } else {
        std::cout << "Return (void)\n";
    }
}

} // namespace dex
