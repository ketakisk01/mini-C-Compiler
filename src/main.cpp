#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "SemanticAnalyzer.h"
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

    cout << "\n========== TOKENS ==========" << "\n";
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
    bool lexError = false;
    for (const auto& token : tokens)
    {
        if (token.type == TokenType::INVALID)
        {
            cerr << "Lexical Error at line " << token.line
                 << ", column " << token.column
                 << ": invalid or unterminated token '" << token.lexeme << "'\n";
            lexError = true;
        }
    }
    if (lexError)
    {
        cerr << "\nLexical error found. Compilation stopped.\n";
        return 1;
    }

    // 2. SYMBOL TABLE INITIALIZATION
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

    // 4. SEMANTIC ANALYSIS
    SemanticAnalyzer semanticAnalyzer(symbolTable);
    if (!semanticAnalyzer.analyze(program, true))
    {
        cerr << "\nSemantic analysis failed. Compilation stopped.\n";
        return 1;
    }

    symbolTable.print();

    // 5. AST OUTPUT
    cout << "\n========== ABSTRACT SYNTAX TREE ==========" << "\n";
    if (program)
    {
        program->print();
    }

    // 6. PARSE TREE OUTPUT
    parser.printParseTree();

    // 7. IR GENERATION
    IRGenerator irGenerator(symbolTable);
    TACProgram tacProgram = irGenerator.generate(program);

    if (irGenerator.hasErrors())
    {
        for (const auto& e : irGenerator.getErrors())
            cerr << "Code Generation Error: " << e << "\n";
        cerr << "\nCompilation stopped.\n";
        return 1;
    }

    cout << "\n========== THREE-ADDRESS CODE (before optimization) ==========" << "\n";
    tacProgram.print();

    // 8. OPTIMIZATION
    Optimizer optimizer;
    TACProgram optimizedProgram = optimizer.optimize(tacProgram);

    cout << "\n========== THREE-ADDRESS CODE (after optimization) ==========" << "\n";
    optimizedProgram.print();

    // 9. CODE GENERATION
    CodeGenerator codeGenerator;
    string assembly = codeGenerator.generateAssembly(optimizedProgram, sourceFileName);

    ofstream outputFile(outputAssemblyFile);
    if (!outputFile)
    {
        cerr << "Error: Cannot open output assembly file '" << outputAssemblyFile << "'.\n";
        return 1;
    }

    outputFile << assembly;
    outputFile.close();

    cout << "\n========== ASSEMBLY OUTPUT ==========" << "\n";
    cout << "Generated assembly written to: " << outputAssemblyFile << "\n";

    return 0;
}