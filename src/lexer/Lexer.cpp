#include "Lexer.h"
#include "magic_enum.hpp"

#include <fmt/core.h>
#include <fstream>

namespace Spine {

Lexer::Lexer(const std::filesystem::path& path) {
    std::fstream stream { path };
    m_tokens.reserve(1024);

    if (!stream.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open source file with path {}.", path.string()));
    }

    std::string line {};
    while (std::getline(stream, line)) {
        m_source += line + '\n';
    }
}

const std::vector<Token>& Lexer::tokenize() {
    if (!m_source.empty()) {
        while (!is_at_end()) {
            handle_tokens();
            m_start = m_current;
        }

        while (m_indents.size() > 1) {
            m_indents.pop_back();
            add_token(TokenType::Dedent, "");
        }

        add_token(TokenType::EndOfFile);
    }
    return m_tokens;
};

void Lexer::handle_tokens() {
    char c = current();
    m_current++;

    switch (c) {
        case '+':
            add_token(TokenType::Plus); break;
        case '-':
            add_token(match('>') ? TokenType::Arrow : TokenType::Minus); break;
        case '*':
            add_token(TokenType::Mul); break;
        case '/':
            add_token(TokenType::Div); break;
        case '(':
            add_token(TokenType::LeftParen); break;
        case ')':
            add_token(TokenType::RightParen); break;
        case ',':
            add_token(TokenType::Comma); break;
        case ';':
            add_token(TokenType::Semicolon); break;
        case ':':
            add_token(TokenType::Colon); break;

        case '=':
            add_token(match('=') ? TokenType::EqEq : TokenType::Eq); break;
        case '<':
            add_token(match('=') ? TokenType::LessEq : TokenType::Less); break;
        case '>':
            add_token(match('=') ? TokenType::GreaterEq : TokenType::Greater); break;
        case '!':
            if (match('=')) add_token(TokenType::NotEq); break;

        case '#':
            while (current() != '\n' && !is_at_end()) m_current++; break;
        
        case '\n':
            new_line(); break;
        
        default:
            if (isspace(c)) {}
            else if (isalpha(c) || c == '_') { identifier(); }
            else if (isdigit(c))  { number(); }
            else if (c == '"') { string(); }
            else { add_token(TokenType::Invalid); }
            break;
    }
}

void Lexer::add_token(TokenType type) {
    m_tokens.emplace_back(Token{substr(), type, m_line});
}

void Lexer::add_token(TokenType type, std::string lexeme) {
    m_tokens.emplace_back(Token{lexeme, type, m_line});
}


void Lexer::identifier() {
    while (isalnum(current()) || current() == '_') {
        m_current++;
    }

    std::string_view id = substr_view();
    auto keyword = magic_enum::enum_cast<TokenType>(id, magic_enum::case_insensitive);

    if (keyword.has_value() && is_keyword(keyword.value()))
        add_token(keyword.value());
    else
        add_token(TokenType::Identifier); 
}

void Lexer::number() {
    while (isdigit(current())) {
        m_current++;
    }

    if (current() == '.') {
        m_current++;
        while (isdigit(current())) {
            m_current++;
        }
    }
    add_token(TokenType::Number);
}

void Lexer::string() {
    m_start = m_current;

    while (!is_at_end() && current() != '"') {
        if (current() == '\n') throw std::runtime_error("Unterminated string");
        m_current++;
    }

    add_token(TokenType::String);
    m_current++;
}

void Lexer::new_line() {
    m_line++;
    int i {0};
    while (!is_at_end() && (current() == ' ' || current() == '\t')) {
        i++;
        m_current++;
    }

    if (is_at_end() || current() == '\n') return;

    if (i > m_indents.back()) {
        m_indents.push_back(i);
        add_token(TokenType::Indent, "");
    }
    else if (i < m_indents.back()) {
        while (m_indents.size() > 1 && i < m_indents.back()) {
            m_indents.pop_back();
            add_token(TokenType::Dedent, "");
        }

        if (m_indents.empty() || m_indents.back() != i) {
            throw std::runtime_error("Invalid indentation level");
        }
    }
    
}


bool Lexer::is_at_end() const {
    return m_current >= m_source.size();
}

char Lexer::current() const {
    if (is_at_end()) return '\0';
    return m_source[m_current];
}

bool Lexer::match(char c) {
    if (current() == c) {
        m_current++;
        return true;
    }
    return false;
}

bool Lexer::is_keyword(TokenType keyword) {
    return std::count(KEYWORDS.begin(), KEYWORDS.end(), keyword) > 0;
}

std::string Lexer::substr() {
    if (is_at_end()) {
        return "";
    }
    return m_source.substr(m_start, m_current-m_start);
}

std::string_view Lexer::substr_view() {
    if (is_at_end()) {
        return "";
    }
    return std::string_view { &m_source[m_start], m_current-m_start };
}

}