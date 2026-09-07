#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "AST.h"
#include "SymbolTable.h"

#include <memory>
#include <string>
#include <vector>

// ==========================================
// Parse Tree Node
// ==========================================

struct ParseTreeNode
{
    std::string name;
    std::vector<std::shared_ptr<ParseTreeNode>> children;

    ParseTreeNode(const std::string& nodeName)
        : name(nodeName)
    {
    }

    void addChild(std::shared_ptr<ParseTreeNode> child)
    {
        if (child)
            children.push_back(child);
    }

    void print(int indent = 0) const;
};

using ParseTreePtr = std::shared_ptr<ParseTreeNode>;


// ==========================================
// Parser
// ==========================================

class Parser
{
private:
    const std::vector<Token>& tokens;
    size_t current;

    SymbolTable& symbolTable;

    ParseTreePtr parseTreeRoot;

    // Stores the tree node produced by the
    // most recently parsed grammar rule.
    ParseTreePtr lastTreeNode;

    // Token helper functions
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

    // Statements
    ASTPtr parseStatement();

    ASTPtr parseDeclaration();
    ASTPtr parseAssignment();
    ASTPtr parseIf();
    ASTPtr parseWhile();
    ASTPtr parseServe();
    ASTPtr parseSip();
    ASTPtr parseReturn();
    ASTPtr parseBlock();

    // Expressions
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

    void printParseTree() const;
};

#endif