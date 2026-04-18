#pragma once

#include <vector>

namespace Spine {

enum class TokenType {
    Invalid,

    Identifier,
    Number,
    String,

    Colon,
    Arrow,
    LeftParen,
    RightParen,
    Comma,
    Semicolon,

    Plus,
    Minus,
    Mul,
    Div,
    Modulo,
    Eq,
    EqEq,
    NotEq,
    Less,
    LessEq,
    Greater,
    GreaterEq,
    And,
    Or,

    Spine,
    Task,
    Run,
    If,
    Fail,
    Rollback,
    Log,

    Copy,
    Move,
    Delete,
    Make,
    Native,

    Var,
    Mode,
    Ignore,
    Live,
    Dry,

    EndOfFile
};

static std::vector<TokenType> KEYWORDS {
    TokenType::Spine,
    TokenType::Task,
    TokenType::Run,
    TokenType::If,
    TokenType::Fail,
    TokenType::Rollback,
    TokenType::Log,
    TokenType::Copy,
    TokenType::Move,
    TokenType::Delete,
    TokenType::Make,
    TokenType::Native,
    TokenType::Var,
    TokenType::Mode,
    TokenType::Ignore,
    TokenType::Live,
    TokenType::Dry,
    TokenType::Or,
    TokenType::And
};

}