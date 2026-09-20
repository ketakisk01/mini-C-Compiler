#ifndef SEMANTIC_ANALYZER_H
#define SEMANTIC_ANALYZER_H

#include "AST.h"
#include "SymbolTable.h"

#include <memory>
#include <string>
#include <vector>

class SemanticAnalyzer
{
private:
    SymbolTable& symbolTable;
    bool ownsSymbolTable;
    std::vector<std::string> errors;

    // Error logging
    void reportError(const std::string& message, int line);

    // AST visitor methods
    void analyzeNode(const ASTPtr& node);
    void analyzeProgram(const std::shared_ptr<ProgramNode>& node);
    void analyzeBlock(const std::shared_ptr<BlockNode>& node);
    void analyzeDeclaration(const std::shared_ptr<DeclarationNode>& node);
    void analyzeAssignment(const std::shared_ptr<AssignmentNode>& node);
    void analyzeIf(const std::shared_ptr<IfNode>& node);
    void analyzeWhile(const std::shared_ptr<WhileNode>& node);
    void analyzeServe(const std::shared_ptr<ServeNode>& node);
    void analyzeSip(const std::shared_ptr<SipNode>& node);
    void analyzeReturn(const std::shared_ptr<ReturnNode>& node);

    // Expression type evaluation
    std::string analyzeExpression(const ASTPtr& expr);
    std::string analyzeBinaryExpr(const std::shared_ptr<BinaryExpressionNode>& node);
    std::string analyzeUnaryExpr(const std::shared_ptr<UnaryExpressionNode>& node);
    std::string analyzeLiteral(const std::shared_ptr<LiteralNode>& node);
    std::string analyzeIdentifier(const std::shared_ptr<IdentifierNode>& node);

    // Type checking helpers
    bool areTypesCompatible(const std::string& targetType, const std::string& exprType) const;
    bool isNumericType(const std::string& type) const;
    bool isBooleanOrNumericType(const std::string& type) const;
    std::string deduceLiteralType(const std::string& value) const;

public:
    explicit SemanticAnalyzer(SymbolTable& table);
    SemanticAnalyzer();
    ~SemanticAnalyzer();

    // Entry point for semantic analysis
    bool analyze(const std::shared_ptr<ProgramNode>& program, bool printOutput = true);

    const std::vector<std::string>& getErrors() const;
    bool hasErrors() const;
    void printReport() const;
};

#endif
