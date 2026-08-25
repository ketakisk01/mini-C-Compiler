#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <memory>
#include "Token.h"
#include "AST.h"
#include "SymbolTable.h"

class Parser {
private:
    std::vector<Token> tokens;
    size_t current;
    SymbolTable& symbolTable;
    std::vector<std::string> errors;

    // Helper token stream functions
    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    Token advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool matchKeyword(const std::string& keyword);
    bool checkKeyword(const std::string& keyword) const;
    Token consume(TokenType type, const std::string& errorMessage);
    Token consumeKeyword(const std::string& keyword, const std::string& errorMessage);
    void synchronize();
    void error(const Token& token, const std::string& message);

    // Parsing statement productions
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseDeclaration();
    std::unique_ptr<ASTNode> parseAssignmentOrExprStmt();
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseWhile();
    std::unique_ptr<ASTNode> parsePrint();
    std::unique_ptr<ASTNode> parseReturn();
    std::unique_ptr<ASTNode> parseBlock();

    // Parsing expression productions (with precedence climbing/cascading)
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseLogicalOr();
    std::unique_ptr<ASTNode> parseLogicalAnd();
    std::unique_ptr<ASTNode> parseEquality();
    std::unique_ptr<ASTNode> parseRelational();
    std::unique_ptr<ASTNode> parseAdditive();
    std::unique_ptr<ASTNode> parseMultiplicative();
    std::unique_ptr<ASTNode> parseUnary();
    std::unique_ptr<ASTNode> parsePostfix();
    std::unique_ptr<ASTNode> parsePrimary();

public:
    Parser(const std::vector<Token>& tokens, SymbolTable& symbolTable);
    std::unique_ptr<Program> parse();
    bool hasErrors() const;
    void printErrors() const;
    const std::vector<std::string>& getErrors() const { return errors; }
};

#endif // PARSER_H
