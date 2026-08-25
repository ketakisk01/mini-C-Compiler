#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <memory>
#include "Token.h"
#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"

// Reads whole file contents into a string
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Aligned printing of the token table
void printTokenTable(const std::vector<Token>& tokens) {
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << std::left << std::setw(15) << "Token Type"
              << std::setw(25) << "Lexeme"
              << std::setw(15) << "Line"
              << std::setw(15) << "Column" << "\n";
    std::cout << "----------------------------------------------------------------------\n";
    for (const auto& token : tokens) {
        std::cout << std::left << std::setw(15) << tokenTypeToString(token.type)
                  << std::setw(25) << token.lexeme
                  << std::setw(15) << token.line
                  << std::setw(15) << token.column << "\n";
    }
    std::cout << "----------------------------------------------------------------------\n";
}

// Aligned printing of the symbol table
void printSymbolTable(const SymbolTable& table) {
    auto symbols = table.getAllSymbols();
    std::cout << "----------------------------------------------------------------------\n";
    std::cout << std::left << std::setw(20) << "Identifier Name"
              << std::setw(20) << "Data Type"
              << std::setw(15) << "Decl Line"
              << std::setw(15) << "Decl Column" << "\n";
    std::cout << "----------------------------------------------------------------------\n";
    if (symbols.empty()) {
        std::cout << "(No symbols declared)\n";
    } else {
        for (const auto& sym : symbols) {
            std::cout << std::left << std::setw(20) << sym.name
                      << std::setw(20) << sym.type
                      << std::setw(15) << sym.line
                      << std::setw(15) << sym.column << "\n";
        }
    }
    std::cout << "----------------------------------------------------------------------\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: MiniCCompiler <source_file.mc>\n";
        return 1;
    }

    std::string filePath = argv[1];
    std::string source;
    try {
        source = readFile(filePath);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    std::cout << "=== MINI C COMPILER ===\n";

    // 1. Lexical Analysis
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "\nLEXICAL ANALYSIS\n";
    printTokenTable(tokens);

    // Track any invalid/lexical errors
    std::vector<Token> lexicalErrors;
    for (const auto& t : tokens) {
        if (t.type == TokenType::INVALID) {
            lexicalErrors.push_back(t);
        }
    }

    // 2. Syntax Analysis and Symbol Table Construction
    SymbolTable symbolTable;
    Parser parser(tokens, symbolTable);
    std::unique_ptr<Program> ast = parser.parse();

    std::cout << "\nSYMBOL TABLE\n";
    printSymbolTable(symbolTable);

    std::cout << "\nSYNTAX ANALYSIS\n";
    if (lexicalErrors.empty() && !parser.hasErrors()) {
        std::cout << "Success: Parsing completed with 0 errors.\n";

        std::cout << "\nAST\n";
        if (ast) {
            ast->print(0);
        }
    } else {
        std::cout << "Failure: Errors detected during compilation.\n";
        if (!lexicalErrors.empty()) {
            std::cout << "\nLexical Errors:\n";
            for (const auto& err : lexicalErrors) {
                std::cout << "  [Lexical Error] Line " << err.line
                          << ", Col " << err.column << ": " << err.lexeme << "\n";
            }
        }
        if (parser.hasErrors()) {
            std::cout << "\nSyntax & Semantic Errors:\n";
            parser.printErrors();
        }
    }

    return (lexicalErrors.empty() && !parser.hasErrors()) ? 0 : 1;
}
