#pragma once

#include <vector>

namespace Spine {

enum class TokenType {
    Invalid,

    Identifier,
    Number,
    String,

    Plus,
    Minus,
    Mul,
    Div,
    LeftParen,
    RightParen,
    LeftBrace,
    RightBrace,
    LeftBracket,
    RightBracket,
    Comma,
    Semicolon,

    Eq,
    EqEq,
    Less,
    LessEq,
    Greater,
    GreaterEq,
    Not,
    NotEq,

    And,
    AndAnd,
    Or,
    OrOr,

    Function,
    Var,
    If,
    Else,
    While,
    Return,
    True,
    False,

    EndOfFile
};

static std::vector<TokenType> KEYWORDS {
    TokenType::Function,
    TokenType::Var,
    TokenType::If,
    TokenType::Else,
    TokenType::While,
    TokenType::Return,
    TokenType::True,
    TokenType::False
};

}