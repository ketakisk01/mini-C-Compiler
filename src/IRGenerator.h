#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H

#include "AST.h"
#include "TAC.h"
#include "SymbolTable.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

class IRGenerator
{
private:
    int tempCount;
    int labelCount;
    TACProgram program;
    const SymbolTable& symbolTable;

    // Scope-aware renaming so shadowed variables get distinct names
    std::vector<std::unordered_map<std::string, std::string>> scopes;
    std::unordered_map<std::string, int> declCount;
    std::vector<std::string> errors;

    std::string declareVar(const std::string& name);
    std::string resolveVar(const std::string& name) const;
    void unsupported(const std::string& what);

    std::string newTemp();
    std::string newLabel();

    void generateStatement(const ASTPtr& node);
    std::string generateExpression(const ASTPtr& node);

public:
    IRGenerator(const SymbolTable& table);

    TACProgram generate(const std::shared_ptr<ProgramNode>& root);

    bool hasErrors() const { return !errors.empty(); }
    const std::vector<std::string>& getErrors() const { return errors; }
};

#endif // IR_GENERATOR_H
