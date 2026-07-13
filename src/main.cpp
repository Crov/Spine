#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "runtime/Interpreter.h"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: spine <source_file_path> [task_name]\n";
        return 1;
    }

    try {
        Spine::Lexer lexer(argv[1]);
        Spine::Parser parser(lexer.tokenize());
        const auto program = parser.parse();
        Spine::Interpreter interpreter;
        if (argc >= 3) {
            interpreter.execute(*program, argv[2]);
        } else {
            interpreter.execute(*program);
        }
        std::cout << "Executed successfully.\n";
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }

    return 0;
}