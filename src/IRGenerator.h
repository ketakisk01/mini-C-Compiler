#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "AST.h"
#include "TAC.h"
#include "SymbolTable.h"
#include <string>
#include <memory>

class IRGenerator
{
private:
    int tempCount;
    int labelCount;
    TACProgram program;
    const SymbolTable& symbolTable;

    std::string newTemp();
    std::string newLabel();

    void generateStatement(const ASTPtr& node);
    std::string generateExpression(const ASTPtr& node);

public:
    IRGenerator(const SymbolTable& table);

    TACProgram generate(const std::shared_ptr<ProgramNode>& root);
};

#endif // IR_GENERATOR_H
