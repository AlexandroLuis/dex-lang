#ifndef DEX_INTERPRETER_H
#define DEX_INTERPRETER_H

#include "../parser/ast.h"
#include <unordered_map>
#include <string>

namespace dex {

class Interpreter {
public:
    Interpreter() = default;

    void interpret(const std::vector<StmtPtr>& statements);

private:
    std::unordered_map<std::string, std::string> variables;  // Simple string vars for now

    void execute(StmtPtr stmt);
    void executeBlock(const std::vector<StmtPtr>& statements);
    std::string evaluate(ExprPtr expr);

    void executeAssign(std::shared_ptr<AssignStmt> stmt);
    void executeExprStmt(std::shared_ptr<ExprStmt> stmt);
    void executeReturn(std::shared_ptr<ReturnStmt> stmt);

    // Add more execute methods as you expand AST and runtime
};

} // namespace dex

#endif // DEX_INTERPRETER_H
