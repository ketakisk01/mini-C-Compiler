#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "SemanticAnalyzer.h"

using namespace std;

int main(int argc, char* argv[])
{
    // Check command-line argument
    if (argc < 2)
    {
        cout << "Usage: MiniCCompiler.exe <source_file>\n";
        return 1;
    }

    // Open source file
    ifstream file(argv[1]);

    if (!file)
    {
        cerr << "Error: Cannot open source file: " << argv[1] << "\n";
        return 1;
    }

    // Read complete source code
    stringstream buffer;
    buffer << file.rdbuf();

    string sourceCode = buffer.str();

    // 1. LEXICAL ANALYSIS

    Lexer lexer(sourceCode);

    vector<Token> tokens = lexer.tokenize();

    cout << "\n========== TOKENS ==========\n";

    for (const auto& token : tokens)
    {
        cout << tokenTypeToString(token.type)
             << " : "
             << token.lexeme
             << "  (Line "
             << token.line
             << ", Column "
             << token.column
             << ")\n";
    }

    cout << "============================\n";


    // Check lexical errors
    for (const auto& token : tokens)
    {
        if (token.type == TokenType::INVALID)
        {
            cerr << "\nLexical error found. Compilation stopped.\n";
            return 1;
        }
    }

    // 2. SYMBOL TABLE

    SymbolTable symbolTable;


    // 3. SYNTAX ANALYSIS

    Parser parser(tokens, symbolTable);

    auto program = parser.parse();

    cout << "\nSyntax analysis completed successfully.\n";


    // 4. SYMBOL TABLE OUTPUT


    symbolTable.print();

    // 5. AST OUTPUT

    cout << "\n========== ABSTRACT SYNTAX TREE ==========\n";

    if (program)
    {
        program->print();
    }


    // 6. PARSE TREE OUTPUT

    parser.printParseTree();


    // 7. SEMANTIC ANALYSIS
    SemanticAnalyzer semanticAnalyzer(symbolTable);
    bool semanticPassed = semanticAnalyzer.analyze(program);

    if (!semanticPassed)
    {
        return 1;
    }

    return 0;
}