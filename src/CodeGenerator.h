#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "TAC.h"
#include <string>
#include <unordered_map>
#include <vector>

class CodeGenerator
{
private:
    std::unordered_map<std::string, int> stackOffsets;
    std::unordered_map<std::string, std::string> stringLabels;
    int nextOffset;
    int stringCount;

    bool isInteger(const std::string& str) const;
    std::string getOperand(const std::string& name);
    int allocateSlot(const std::string& name);

public:
    CodeGenerator();

    std::string generateAssembly(const TACProgram& prog, const std::string& filename = "program.mc");
};

#endif // CODE_GENERATOR_H
