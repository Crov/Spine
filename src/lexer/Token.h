#pragma once

#include "TokenType.h"

#include <string>

namespace Spine {

struct Token {
    std::string lexeme {};
    TokenType type {};
    size_t line {};
};

}