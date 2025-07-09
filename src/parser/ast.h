// src/parser/ast.h
#ifndef DEX_AST_H
#define DEX_AST_H
#include <string>
#include <vector>
#include <memory>

namespace dex {

struct Expr;
struct Stmt;
using ExprPtr = std::shared_ptr<Expr>;
using StmtPtr = std::shared_ptr<Stmt>;

// Base expression
struct Expr {
    virtual ~Expr() = default;
};

// Base statement
struct Stmt {
    virtual ~Stmt() = default;
};

// Literal expressions (string, number)
struct LiteralExpr : Expr {
    std::string value;
    LiteralExpr(const std::string& val) : value(val) {}
};

// Variable expressions (identifier)
struct VariableExpr : Expr {
    std::string name;
    VariableExpr(const std::string& n) : name(n) {}
};

// Member access expression: object.property
struct MemberAccessExpr : Expr {
    ExprPtr object;
    std::string property;
    MemberAccessExpr(ExprPtr obj, const std::string& prop)
        : object(std::move(obj)), property(prop) {}
};


// Function call expressions: callee(args...)
struct CallExpr : Expr {
    ExprPtr callee;
    std::vector<ExprPtr> arguments;
    CallExpr(ExprPtr c, std::vector<ExprPtr> args)
        : callee(std::move(c)), arguments(std::move(args)) {}
};

// Anonymous function expressions (func() { ... })
struct FuncExpr : Expr {
    std::vector<std::string> params;  // For now, empty or can extend later
    std::vector<StmtPtr> body;
};

// Assignment statement: variable = expression
struct AssignStmt : Stmt {
    std::string name;
    ExprPtr value;
    AssignStmt(const std::string& n, ExprPtr v) : name(n), value(std::move(v)) {}
};

// Expression statement: expr;
struct ExprStmt : Stmt {
    ExprPtr expression;
    ExprStmt(ExprPtr expr) : expression(std::move(expr)) {}
};

// Return statement
struct ReturnStmt : Stmt {
    ExprPtr value;
    ReturnStmt(ExprPtr v) : value(std::move(v)) {}
};

// Block statement { ... }
struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
    BlockStmt(std::vector<StmtPtr> stmts) : statements(std::move(stmts)) {}
};

// If statement: if (cond) then branch else optional else branch
struct IfStmt : Stmt {
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;  // optional

    IfStmt(ExprPtr cond, StmtPtr then, StmtPtr else_branch = nullptr)
        : condition(std::move(cond)), thenBranch(std::move(then)), elseBranch(std::move(else_branch)) {}
};

// While statement: while (cond) body
struct WhileStmt : Stmt {
    ExprPtr condition;
    StmtPtr body;

    WhileStmt(ExprPtr cond, StmtPtr b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

}  // namespace dex

#endif  // DEX_AST_H
