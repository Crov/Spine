#pragma once

#include "Ast.h"
#include "../lexer/Token.h"

#include <vector>

namespace Spine {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::unique_ptr<BlockStatement> parse();

private:
    StatementPtr parse_statement();
    StatementPtr parse_variable();
    StatementPtr parse_task();
    StatementPtr parse_if();
    StatementPtr parse_action();
    std::unique_ptr<BlockStatement> parse_block();

    ExpressionPtr parse_expression();
    ExpressionPtr parse_binary(int minimum_precedence);
    ExpressionPtr parse_primary();

    void skip_empty_lines();
    bool is_action(TokenType type) const;
    int precedence(TokenType type) const;
    const Token& consume(TokenType type, const char* message);
    bool match(TokenType type);
    bool check(TokenType type) const;
    const Token& advance();
    [[noreturn]] void error(const Token& token, const char* message) const;

    const std::vector<Token>& m_tokens;
    size_t m_current {};
};

}
