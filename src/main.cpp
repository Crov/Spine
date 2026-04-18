#include "lexer/Lexer.h"
#include "magic_enum.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: spien <source_file_path\n";
        return 1;
    }

    std::filesystem::path path = "C:/Users/User/Dev/Spine/scripts/test.spine";

    Spine::Lexer lexer(path);

    for (const auto& token : lexer.tokenize()) {
        std::cout << token.lexeme << "\t(" << magic_enum::enum_name(token.type) << ")\n";
    }

    return 0;
}