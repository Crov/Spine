#pragma once

#include "../parser/Ast.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Spine {

using Value = std::variant<double, std::string, bool>;

class Interpreter {
public:
    void execute(const BlockStatement& program);
    void execute(const BlockStatement& program, const std::string& task_name);

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
    void execute_program(const BlockStatement& program);
    void register_tasks(const BlockStatement& block);
    void register_task(const TaskStatement& task);
    const TaskStatement* find_task(const std::string& name) const;
    bool is_truthy(const Value& value) const;
    std::string value_to_string(const Value& value) const;

    std::unique_ptr<Environment> m_global;
    Environment* m_current {};
    std::vector<const TaskStatement*> m_tasks;
};

}
