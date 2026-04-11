#pragma once

#include "Token.h"

#include <filesystem>
#include <iostream>

namespace Spine {

class Lexer {
public:
    Lexer(const std::filesystem::path& path);

    const std::vector<Token>& tokenize();

private:
    void handle_tokens();
    void add_token(TokenType type);

    void identifier();
    void number();
    void string();

private:
    bool is_at_end() const;
    char current() const;
    bool match(char c);
    std::string substr();

private:
    std::string m_source {};
    std::vector<Token> m_tokens {};

    size_t m_start {};
    size_t m_current {};
    size_t m_line {};
};

}
