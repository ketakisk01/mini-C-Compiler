#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "AST.h"
#include "SymbolTable.h"

#include <vector>
#include <memory>
#include <string>

class Parser {
private:

    std::vector<Token> tokens;

    size_t current;

    SymbolTable& symbolTable;

    Token& peek();

    Token& previous();

    bool check(TokenType type);

    bool match(TokenType type);

    Token consume(
        TokenType type,
        const std::string& message
    );

    void syntaxError(
        const std::string& message
    );

    ASTPtr parseStatement();

    ASTPtr parseDeclaration();

    ASTPtr parseAssignment();

    ASTPtr parseIf();

    ASTPtr parseWhile();

    ASTPtr parseServe();

    ASTPtr parseSip();

    ASTPtr parseReturn();

    ASTPtr parseBlock();

    ASTPtr parseExpression();

    ASTPtr parseLogicalOr();

    ASTPtr parseLogicalAnd();

    ASTPtr parseEquality();

    ASTPtr parseComparison();

    ASTPtr parseTerm();

    ASTPtr parseFactor();

    ASTPtr parseUnary();

    ASTPtr parsePrimary();

public:

    Parser(
        const std::vector<Token>& tokenList,
        SymbolTable& table
    );

    std::shared_ptr<ProgramNode> parse();
};

#endif