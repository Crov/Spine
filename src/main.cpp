#include "lexer/Lexer.h"
#include "parser/Parser.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: spine <source_file_path>\n";
        return 1;
    }

    try {
        Spine::Lexer lexer(argv[1]);
        Spine::Parser parser(lexer.tokenize());
        parser.parse();
        std::cout << "Parsed successfully.\n";
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}