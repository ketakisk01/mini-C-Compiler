#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include "TAC.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

struct OptimizationReport
{
    int constantFolds = 0;
    int algebraicSimplifications = 0;
    int constantPropagations = 0;
    int deadInstructionsRemoved = 0;
};

class Optimizer
{
private:
    OptimizationReport report;

    bool isInteger(const std::string& str) const;
    long long parseInteger(const std::string& str) const;
    bool isTemp(const std::string& str) const;

    bool passConstantFoldingAndAlgebraic(TACProgram& prog);
    bool passConstantPropagation(TACProgram& prog);
    bool passDeadCodeElimination(TACProgram& prog);

public:
    Optimizer();

    TACProgram optimize(const TACProgram& inputProg);

    const OptimizationReport& getReport() const
    {
        return report;
    }
};

#endif // OPTIMIZER_H
