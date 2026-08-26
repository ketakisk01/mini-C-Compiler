#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char* argv[]) {

    std::cout
        << "========================================\n"
        << "          COFFEE++ COMPILER\n"
        << "========================================\n\n";

    if (argc < 2) {

        std::cout
            << "Usage:\n"
            << "  MiniCCompiler.exe <source-file>\n\n";

        std::cout
            << "Example:\n"
            << "  MiniCCompiler.exe examples\\valid_program.mc\n";

        return 1;
    }

    std::string filename = argv[1];

    std::ifstream file(filename);

    if (!file.is_open()) {

        std::cerr
            << "Error: Cannot open file: "
            << filename
            << "\n";

        return 1;
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    std::string sourceCode = buffer.str();

    file.close();

    std::cout
        << "Input File: "
        << filename
        << "\n\n";

    // ==============================
    // LEXICAL ANALYSIS
    // ==============================

    std::cout
        << "========== LEXICAL ANALYSIS ==========\n";

    Lexer lexer(sourceCode);

    std::vector<Token> tokens = lexer.tokenize();

    bool lexicalError = false;

    std::cout
        << "\n"
        << "Type\t\tLexeme\t\tLine\tColumn\n";

    std::cout
        << "-----------------------------------------------\n";

    for (const Token& token : tokens) {

        std::cout
            << tokenTypeToString(token.type)
            << "\t\t"
            << token.lexeme
            << "\t\t"
            << token.line
            << "\t"
            << token.column
            << "\n";

        if (token.type == TokenType::INVALID) {
            lexicalError = true;
        }
    }

    if (lexicalError) {

        std::cerr
            << "\nLexical analysis failed.\n";

        return 1;
    }

    std::cout
        << "\nLexical analysis successful.\n";

    // ==============================
    // SYMBOL TABLE
    // ==============================

    SymbolTable symbolTable;

    // ==============================
    // SYNTAX ANALYSIS
    // ==============================

    std::cout
        << "\n========== SYNTAX ANALYSIS ==========\n";

    Parser parser(
        tokens,
        symbolTable
    );

    std::shared_ptr<ProgramNode> program =
        parser.parse();

    std::cout
        << "\nSyntax analysis completed.\n";

    // ==============================
    // SYMBOL TABLE
    // ==============================

    symbolTable.print();

    // ==============================
    // AST
    // ==============================

    std::cout
        << "\n========== ABSTRACT SYNTAX TREE ==========\n";

    program->print();

    std::cout
        << "\n========================================\n"
        << "       COFFEE++ COMPILATION DONE\n"
        << "========================================\n";

    return 0;
}