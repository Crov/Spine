#pragma once

#include "../parser/Ast.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace Spine {

using Value = std::variant<double, std::string, bool>;

class Interpreter {
public:
    void execute(const BlockStatement& program);

private:
    struct Environment {
        std::unordered_map<std::string, Value> values;
        Environment* parent {};

        void define(const std::string& name, Value value);
        Value get(const std::string& name) const;
    };

    Value evaluate(const Expression& expression) const;
    void execute(const Statement& statement);
    void execute_block(const BlockStatement& block);
    bool is_truthy(const Value& value) const;
    std::string value_to_string(const Value& value) const;

    std::unique_ptr<Environment> m_global;
    Environment* m_current {};
};

}
