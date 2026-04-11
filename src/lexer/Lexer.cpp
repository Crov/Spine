#include "Lexer.h"

#include <fmt/core.h>
#include <fstream>

namespace Spine {

Lexer::Lexer(const std::filesystem::path& path) {
    std::fstream stream { path };

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
            add_token(TokenType::Minus); break;
        case '*':
            add_token(TokenType::Mul); break;
        case '/':
            add_token(TokenType::Div); break;
        case '(':
            add_token(TokenType::LeftParen); break;
        case ')':
            add_token(TokenType::RightParen); break;
        case '{':
            add_token(TokenType::LeftBrace); break;
        case '}':
            add_token(TokenType::RightBrace); break;
        case '[':
            add_token(TokenType::LeftBracket); break;
        case ']':
            add_token(TokenType::RightBracket); break;
        case ',':
            add_token(TokenType::Comma); break;
        case ';':
            add_token(TokenType::Semicolon); break;

        case '=':
            add_token(match('=') ? TokenType::EqEq : TokenType::Eq); break;
        case '<':
            add_token(match('=') ? TokenType::LessEq : TokenType::Less); break;
        case '>':
            add_token(match('=') ? TokenType::GreaterEq : TokenType::Greater); break;
        case '!':
            add_token(match('=') ? TokenType::NotEq : TokenType::Not); break;
        case '&':
            add_token(match('&') ? TokenType::AndAnd : TokenType::And); break;
        case '|':
            add_token(match('|') ? TokenType::OrOr : TokenType::Or); break;

        case '\n':
            m_line++; break;

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


void Lexer::identifier() {
    while (isalnum(current()) || current() == '_') {
        m_current++;
    }
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

std::string Lexer::substr() {
    if (is_at_end()) {
        return "";
    }
    return m_source.substr(m_start, m_current-m_start);
}

}