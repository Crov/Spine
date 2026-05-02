#pragma once

#include "../lexer/TokenType.h"

#include <memory>
#include <string>
#include <variant>
#include <utility>
#include <vector>

namespace Spine {

struct Expression {
    virtual ~Expression() = default;
};

using ExpressionPtr = std::unique_ptr<Expression>;

struct LiteralExpression final : Expression {
    explicit LiteralExpression(std::variant<double, std::string> value)
        : value(std::move(value)) {}

    std::variant<double, std::string> value;
};

struct VariableExpression final : Expression {
    explicit VariableExpression(std::string name) : name(std::move(name)) {}

    std::string name;
};

struct BinaryExpression final : Expression {
    BinaryExpression(ExpressionPtr left, TokenType operation, ExpressionPtr right)
        : left(std::move(left)), operation(operation), right(std::move(right)) {}

    ExpressionPtr left;
    TokenType operation;
    ExpressionPtr right;
};

struct Statement {
    virtual ~Statement() = default;
};

using StatementPtr = std::unique_ptr<Statement>;

struct BlockStatement final : Statement {
    std::vector<StatementPtr> statements;
};

struct VariableStatement final : Statement {
    std::string name;
    ExpressionPtr value;
};

struct TaskStatement final : Statement {
    std::string name;
    std::unique_ptr<BlockStatement> body;
};

struct IfStatement final : Statement {
    ExpressionPtr condition;
    std::unique_ptr<BlockStatement> body;
};

struct ActionStatement final : Statement {
    TokenType action;
    std::vector<ExpressionPtr> arguments;
};

}
