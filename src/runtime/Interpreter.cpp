#include "Interpreter.h"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace Spine {

void Interpreter::execute(const BlockStatement& program) {
    m_global = std::make_unique<Environment>();
    m_current = m_global.get();
    m_tasks.clear();
    register_tasks(program);
    execute_program(program);
}

void Interpreter::execute(
    const BlockStatement& program,
    const std::string& task_name
) {
    execute(program);
    const auto* task = find_task(task_name);
    if (!task) {
        throw std::runtime_error("Unknown task: " + task_name);
    }

    execute_block(*task->body);
}

void Interpreter::Environment::define(const std::string& name, Value value) {
    values[name] = std::move(value);
}

Value Interpreter::Environment::get(const std::string& name) const {
    auto value = values.find(name);
    if (value != values.end()) {
        return value->second;
    }

    if (parent) {
        return parent->get(name);
    }

    throw std::runtime_error("Undefined variable: " + name);
}

Value Interpreter::evaluate(const Expression& expression) const {
    auto* literal = dynamic_cast<const LiteralExpression*>(&expression);
    if (literal) {
        if (std::holds_alternative<double>(literal->value)) {
            return std::get<double>(literal->value);
        }
        return std::get<std::string>(literal->value);
    }

    auto* variable = dynamic_cast<const VariableExpression*>(&expression);
    if (variable) {
        return m_current->get(variable->name);
    }

    auto* binary = dynamic_cast<const BinaryExpression*>(&expression);
    if (!binary) {
        throw std::runtime_error("Unknown expression");
    }

    Value left = evaluate(*binary->left);
    Value right = evaluate(*binary->right);

    if (binary->operation == TokenType::Plus &&
        (std::holds_alternative<std::string>(left) ||
         std::holds_alternative<std::string>(right))) {
        return value_to_string(left) + value_to_string(right);
    }

    switch (binary->operation) {
        case TokenType::EqEq: return left == right;
        case TokenType::NotEq: return left != right;
        case TokenType::And: return is_truthy(left) && is_truthy(right);
        case TokenType::Or: return is_truthy(left) || is_truthy(right);
        default: break;
    }

    if (!std::holds_alternative<double>(left) || !std::holds_alternative<double>(right)) {
        throw std::runtime_error("Numeric operator requires numeric operands");
    }

    double left_number = std::get<double>(left);
    double right_number = std::get<double>(right);
    switch (binary->operation) {
        case TokenType::Plus: return left_number + right_number;
        case TokenType::Minus: return left_number - right_number;
        case TokenType::Mul: return left_number * right_number;
        case TokenType::Div:
            if (right_number == 0) {
                throw std::runtime_error("Division by zero");
            }
            return left_number / right_number;
        case TokenType::Modulo:
            if (right_number == 0) {
                throw std::runtime_error("Modulo by zero");
            }
            return std::fmod(left_number, right_number);
        case TokenType::Less: return left_number < right_number;
        case TokenType::LessEq: return left_number <= right_number;
        case TokenType::Greater: return left_number > right_number;
        case TokenType::GreaterEq: return left_number >= right_number;
        default: break;
    }

    throw std::runtime_error("Unsupported binary operator");
}

void Interpreter::execute(const Statement& statement) {
    auto* block = dynamic_cast<const BlockStatement*>(&statement);
    if (block) {
        execute_block(*block);
        return;
    }

    auto* variable = dynamic_cast<const VariableStatement*>(&statement);
    if (variable) {
        m_current->define(variable->name, evaluate(*variable->value));
        return;
    }

    auto* condition = dynamic_cast<const IfStatement*>(&statement);
    if (condition) {
        if (is_truthy(evaluate(*condition->condition))) {
            execute_block(*condition->body);
        }
        return;
    }

    if (dynamic_cast<const TaskStatement*>(&statement) ||
        dynamic_cast<const ActionStatement*>(&statement)) {
        return;
    }

    throw std::runtime_error("Unknown statement");
}

void Interpreter::execute_block(const BlockStatement& block) {
    Environment local;
    local.parent = m_current;
    m_current = &local;

    for (const auto& statement : block.statements) {
        execute(*statement);
    }

    m_current = local.parent;
}

void Interpreter::execute_program(const BlockStatement& program) {
    for (const auto& statement : program.statements) {
        auto* block = dynamic_cast<const BlockStatement*>(statement.get());
        if (block) {
            for (const auto& nested_statement : block->statements) {
                if (!dynamic_cast<const TaskStatement*>(nested_statement.get())) {
                    execute(*nested_statement);
                }
            }
        } else if (!dynamic_cast<const TaskStatement*>(statement.get())) {
            execute(*statement);
        }
    }
}

void Interpreter::register_tasks(const BlockStatement& block) {
    for (const auto& statement : block.statements) {
        auto* nested_block = dynamic_cast<const BlockStatement*>(statement.get());
        if (nested_block) {
            register_tasks(*nested_block);
            continue;
        }

        auto* task = dynamic_cast<const TaskStatement*>(statement.get());
        if (task) {
            register_task(*task);
        }
    }
}

void Interpreter::register_task(const TaskStatement& task) {
    m_tasks.push_back(&task);
    register_tasks(*task.body);
}

const TaskStatement* Interpreter::find_task(const std::string& name) const {
    for (const auto* task : m_tasks) {
        if (task->name == name) {
            return task;
        }
    }

    return nullptr;
}

bool Interpreter::is_truthy(const Value& value) const {
    if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    }
    if (std::holds_alternative<double>(value)) {
        return std::get<double>(value) != 0;
    }
    return !std::get<std::string>(value).empty();
}

std::string Interpreter::value_to_string(const Value& value) const {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    }
    return std::get<bool>(value) ? "true" : "false";
}

}
