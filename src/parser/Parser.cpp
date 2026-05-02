#include "Parser.h"

#include <stdexcept>

namespace Spine {

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

std::unique_ptr<BlockStatement> Parser::parse() {
    auto program = std::make_unique<BlockStatement>();
    skip_empty_lines();

    if (match(TokenType::Spine)) {
        consume(TokenType::Colon, "Expected ':' after 'spine'");
        program->statements.push_back(parse_block());
    }

    while (!check(TokenType::EndOfFile)) {
        program->statements.push_back(parse_statement());
        skip_empty_lines();
    }

    consume(TokenType::EndOfFile, "Expected end of file");
    return program;
}

StatementPtr Parser::parse_statement() {
    if (check(TokenType::Var)) return parse_variable();
    if (check(TokenType::Task)) return parse_task();
    if (check(TokenType::If)) return parse_if();
    if (is_action(m_tokens[m_current].type)) return parse_action();

    error(m_tokens[m_current], "Expected a statement");
}

StatementPtr Parser::parse_variable() {
    advance();
    const auto& name = consume(TokenType::Identifier, "Expected variable name");
    consume(TokenType::Eq, "Expected '=' after variable name");

    auto variable = std::make_unique<VariableStatement>();
    variable->name = name.lexeme;
    variable->value = parse_expression();
    return variable;
}

StatementPtr Parser::parse_task() {
    advance();
    const auto& name = consume(TokenType::String, "Expected task name");
    consume(TokenType::Colon, "Expected ':' after task name");

    auto task = std::make_unique<TaskStatement>();
    task->name = name.lexeme;
    task->body = parse_block();
    return task;
}

StatementPtr Parser::parse_if() {
    advance();

    auto condition = std::make_unique<IfStatement>();
    condition->condition = parse_expression();
    consume(TokenType::Colon, "Expected ':' after condition");
    condition->body = parse_block();
    return condition;
}

StatementPtr Parser::parse_action() {
    auto action = std::make_unique<ActionStatement>();
    action->action = advance().type;

    const auto argument_count = action->action == TokenType::Copy ||
        action->action == TokenType::Move ? 2 : 1;

    for (int i = 0; i < argument_count; ++i) {
        action->arguments.push_back(parse_expression());
        if (i + 1 < argument_count) {
            consume(TokenType::Arrow, "Expected '->' between action arguments");
        }
    }

    return action;
}

std::unique_ptr<BlockStatement> Parser::parse_block() {
    consume(TokenType::Indent, "Expected an indented block");

    auto block = std::make_unique<BlockStatement>();
    skip_empty_lines();
    while (!check(TokenType::Dedent) && !check(TokenType::EndOfFile)) {
        block->statements.push_back(parse_statement());
        skip_empty_lines();
    }

    consume(TokenType::Dedent, "Expected end of indented block");
    return block;
}

ExpressionPtr Parser::parse_expression() {
    return parse_binary(1);
}

ExpressionPtr Parser::parse_binary(int minimum_precedence) {
    auto left = parse_primary();

    while (true) {
        const auto operation = m_tokens[m_current].type;
        const auto operation_precedence = precedence(operation);
        if (operation_precedence < minimum_precedence) break;

        advance();
        auto right = parse_binary(operation_precedence + 1);
        left = std::make_unique<BinaryExpression>(
            BinaryExpression{std::move(left), operation, std::move(right)});
    }

    return left;
}

ExpressionPtr Parser::parse_primary() {
    if (match(TokenType::Number)) {
        return std::make_unique<LiteralExpression>(
            LiteralExpression{std::stod(m_tokens[m_current - 1].lexeme)});
    }

    if (match(TokenType::String)) {
        return std::make_unique<LiteralExpression>(
            LiteralExpression{m_tokens[m_current - 1].lexeme});
    }

    if (match(TokenType::Identifier)) {
        return std::make_unique<VariableExpression>(
            VariableExpression{m_tokens[m_current - 1].lexeme});
    }

    if (match(TokenType::LeftParen)) {
        auto expression = parse_expression();
        consume(TokenType::RightParen, "Expected ')' after expression");
        return expression;
    }

    error(m_tokens[m_current], "Expected an expression");
}

void Parser::skip_empty_lines() {
    while (match(TokenType::Semicolon)) {}
}

bool Parser::is_action(TokenType type) const {
    return type == TokenType::Copy ||
        type == TokenType::Move ||
        type == TokenType::Delete ||
        type == TokenType::Make ||
        type == TokenType::Run ||
        type == TokenType::Log ||
        type == TokenType::Fail ||
        type == TokenType::Rollback;
}

int Parser::precedence(TokenType type) const {
    switch (type) {
        case TokenType::Or: return 1;
        case TokenType::And: return 2;
        case TokenType::EqEq:
        case TokenType::NotEq: return 3;
        case TokenType::Less:
        case TokenType::LessEq:
        case TokenType::Greater:
        case TokenType::GreaterEq: return 4;
        case TokenType::Plus:
        case TokenType::Minus: return 5;
        case TokenType::Mul:
        case TokenType::Div:
        case TokenType::Modulo: return 6;
        default: return 0;
    }
}

const Token& Parser::consume(TokenType type, const char* message) {
    if (check(type)) return advance();
    error(m_tokens[m_current], message);
}

bool Parser::match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

bool Parser::check(TokenType type) const {
    return m_tokens[m_current].type == type;
}

const Token& Parser::advance() {
    if (!check(TokenType::EndOfFile)) ++m_current;
    return m_tokens[m_current - 1];
}

void Parser::error(const Token& token, const char* message) const {
    throw std::runtime_error(
        "Line " + std::to_string(token.line + 1) + ": " + message);
}

}