#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "TAC.h"
#include "IRGenerator.h"
#include "Optimizer.h"
#include "CodeGenerator.h"

using namespace std;

int main(int argc, char* argv[])
{
    // Check command-line argument
    if (argc < 2)
    {
        cout << "Usage: MiniCCompiler.exe <source_file> [-o <output.s>]\n";
        return 1;
    }

    string sourceFileName = argv[1];
    string outputAssemblyFile = "";

    for (int i = 2; i < argc; ++i)
    {
        if (string(argv[i]) == "-o" && i + 1 < argc)
        {
            outputAssemblyFile = argv[++i];
        }
    }

    if (outputAssemblyFile.empty())
    {
        size_t dotPos = sourceFileName.find_last_of('.');
        if (dotPos != string::npos)
        {
            outputAssemblyFile = sourceFileName.substr(0, dotPos) + ".s";
        }
        else
        {
            outputAssemblyFile = sourceFileName + ".s";
        }
    }

    // Open source file
    ifstream file(sourceFileName);

    if (!file)
    {
        cerr << "Error: Cannot open source file.\n";
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

    if (parser.hasErrors())
    {
        cerr << "\nSyntax analysis completed with " << parser.getErrorCount() << " error(s). Compilation stopped.\n";
        return 1;
    }

    cout << "\nSyntax analysis completed successfully with 0 errors.\n";

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


    return 0;
}