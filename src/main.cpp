#include "lexer/Lexer.h"
#include "magic_enum.hpp"

#include <iostream>

int main() {
    std::filesystem::path path = "C:/Users/User/Dev/Spine/scripts/running/test.spine";

    Spine::Lexer lexer(path);

    for (const auto& token : lexer.tokenize()) {
        std::cout << token.lexeme << "\t(" << magic_enum::enum_name(token.type) << ")\n";
    }
}