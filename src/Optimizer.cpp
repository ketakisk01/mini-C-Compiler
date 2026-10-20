#include "Optimizer.h"
#include <cctype>
#include <iostream>

Optimizer::Optimizer()
{
}

bool Optimizer::isInteger(const std::string& str) const
{
    if (str.empty())
        return false;

    size_t start = 0;
    if (str[0] == '-' || str[0] == '+')
    {
        if (str.length() == 1)
            return false;
        start = 1;
    }

    for (size_t i = start; i < str.length(); ++i)
    {
        if (!isdigit(static_cast<unsigned char>(str[i])))
            return false;
    }
    return true;
}

long long Optimizer::parseInteger(const std::string& str) const
{
    try
    {
        return std::stoll(str);
    }
    catch (...)
    {
        return 0;
    }
}

bool Optimizer::isTemp(const std::string& str) const
{
    if (str.length() >= 2 && str[0] == 't')
    {
        for (size_t i = 1; i < str.length(); ++i)
        {
            if (!isdigit(static_cast<unsigned char>(str[i])))
                return false;
        }
        return true;
    }
    return false;
}

bool Optimizer::passConstantFoldingAndAlgebraic(TACProgram& prog)
{
    bool changed = false;

    for (auto& inst : prog.instructions)
    {
        // 1. Constant folding for binary operators
        if (isInteger(inst.arg1) && isInteger(inst.arg2))
        {
            long long v1 = parseInteger(inst.arg1);
            long long v2 = parseInteger(inst.arg2);
            long long res = 0;
            bool canFold = true;

            switch (inst.op)
            {
                case TACOp::ADD: res = v1 + v2; break;
                case TACOp::SUB: res = v1 - v2; break;
                case TACOp::MUL: res = v1 * v2; break;
                case TACOp::DIV:
                    if (v2 != 0) res = v1 / v2;
                    else canFold = false;
                    break;
                case TACOp::MOD:
                    if (v2 != 0) res = v1 % v2;
                    else canFold = false;
                    break;
                case TACOp::EQ: res = (v1 == v2) ? 1 : 0; break;
                case TACOp::NE: res = (v1 != v2) ? 1 : 0; break;
                case TACOp::LT: res = (v1 < v2) ? 1 : 0; break;
                case TACOp::GT: res = (v1 > v2) ? 1 : 0; break;
                case TACOp::LE: res = (v1 <= v2) ? 1 : 0; break;
                case TACOp::GE: res = (v1 >= v2) ? 1 : 0; break;
                case TACOp::AND: res = ((v1 != 0) && (v2 != 0)) ? 1 : 0; break;
                case TACOp::OR:  res = ((v1 != 0) || (v2 != 0)) ? 1 : 0; break;
                default: canFold = false; break;
            }

            if (canFold)
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = std::to_string(res);
                inst.arg2 = "";
                changed = true;
                report.constantFolds++;
                continue;
            }
        }

        // 2. Constant folding for unary operators
        if (isInteger(inst.arg1))
        {
            long long v1 = parseInteger(inst.arg1);
            if (inst.op == TACOp::NEG)
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = std::to_string(-v1);
                inst.arg2 = "";
                changed = true;
                report.constantFolds++;
                continue;
            }
            else if (inst.op == TACOp::NOT)
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = (v1 == 0) ? "1" : "0";
                inst.arg2 = "";
                changed = true;
                report.constantFolds++;
                continue;
            }
        }

        // 3. Algebraic Simplification
        if (inst.op == TACOp::ADD)
        {
            if (inst.arg2 == "0")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg2 = "";
                changed = true;
                report.algebraicSimplifications++;
            }
            else if (inst.arg1 == "0")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = inst.arg2;
                inst.arg2 = "";
                changed = true;
                report.algebraicSimplifications++;
            }
        }
        else if (inst.op == TACOp::SUB)
        {
            if (inst.arg2 == "0")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg2 = "";
                changed = true;
                report.algebraicSimplifications++;
            }
            else if (inst.arg1 == inst.arg2 && !inst.arg1.empty())
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = "0";
                inst.arg2 = "";
                changed = true;
                report.algebraicSimplifications++;
            }
        }
        else if (inst.op == TACOp::MUL)
        {
            if (inst.arg2 == "1")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg2 = "";
                changed = true;
                report.algebraicSimplifications++;
            }
            else if (inst.arg1 == "1")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = inst.arg2;
                inst.arg2 = "";
                changed = true;
                report.algebraicSimplifications++;
            }
            else if (inst.arg2 == "0" || inst.arg1 == "0")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg1 = "0";
                inst.arg2 = "";
                changed = true;
                report.algebraicSimplifications++;
            }
        }
        else if (inst.op == TACOp::DIV)
        {
            if (inst.arg2 == "1")
            {
                inst.op = TACOp::ASSIGN;
                inst.arg2 = "";
                changed = true;
                report.algebraicSimplifications++;
            }
        }
        else if (inst.op == TACOp::EQ && inst.arg1 == inst.arg2 && !inst.arg1.empty())
        {
            inst.op = TACOp::ASSIGN;
            inst.arg1 = "1";
            inst.arg2 = "";
            changed = true;
            report.algebraicSimplifications++;
        }
        else if (inst.op == TACOp::NE && inst.arg1 == inst.arg2 && !inst.arg1.empty())
        {
            inst.op = TACOp::ASSIGN;
            inst.arg1 = "0";
            inst.arg2 = "";
            changed = true;
            report.algebraicSimplifications++;
        }
    }

    return changed;
}

bool Optimizer::passConstantPropagation(TACProgram& prog)
{
    bool changed = false;
    std::unordered_map<std::string, std::string> knownConstants;

    for (auto& inst : prog.instructions)
    {
        // Clear constant map across label entry
        if (inst.op == TACOp::LABEL)
        {
            knownConstants.clear();
            continue;
        }

        if (inst.op == TACOp::SIP)
        {
            knownConstants.erase(inst.result);
            continue;
        }

        // Propagate in arg1
        if (!inst.arg1.empty() && knownConstants.count(inst.arg1))
        {
            inst.arg1 = knownConstants[inst.arg1];
            changed = true;
            report.constantPropagations++;
        }

        // Propagate in arg2
        if (!inst.arg2.empty() && knownConstants.count(inst.arg2))
        {
            inst.arg2 = knownConstants[inst.arg2];
            changed = true;
            report.constantPropagations++;
        }

        // Control transfer exits current basic block
        if (inst.op == TACOp::GOTO ||
            inst.op == TACOp::IF_TRUE ||
            inst.op == TACOp::IF_FALSE ||
            inst.op == TACOp::RETURN)
        {
            knownConstants.clear();
            continue;
        }

        // Record constant assignment or clear invalidated destination
        if (inst.op == TACOp::ASSIGN)
        {
            if (isInteger(inst.arg1))
            {
                knownConstants[inst.result] = inst.arg1;
            }
            else if (knownConstants.count(inst.arg1))
            {
                knownConstants[inst.result] = knownConstants[inst.arg1];
            }
            else
            {
                knownConstants.erase(inst.result);
            }
        }
        else if (!inst.result.empty())
        {
            knownConstants.erase(inst.result);
        }
    }

    return changed;
}

bool Optimizer::passDeadCodeElimination(TACProgram& prog)
{
    bool changed = false;
    std::vector<TACInstruction> newInsts;

    // 1. Simplify constant conditional branches and remove unreachable code after unconditional jumps
    bool unreachable = false;
    for (size_t i = 0; i < prog.instructions.size(); ++i)
    {
        auto inst = prog.instructions[i];

        if (inst.op == TACOp::LABEL)
        {
            unreachable = false;
            newInsts.push_back(inst);
            continue;
        }

        if (unreachable)
        {
            changed = true;
            report.deadInstructionsRemoved++;
            continue;
        }

        // Branch simplification with constant conditions
        if (inst.op == TACOp::IF_FALSE && isInteger(inst.arg1))
        {
            long long val = parseInteger(inst.arg1);
            if (val == 0)
            {
                // Condition is 0 (false), so branch is taken!
                inst.op = TACOp::GOTO;
                inst.arg1 = "";
                changed = true;
                newInsts.push_back(inst);
                unreachable = true;
                continue;
            }
            else
            {
                // Condition is non-zero (true), branch is never taken!
                changed = true;
                report.deadInstructionsRemoved++;
                continue;
            }
        }
        else if (inst.op == TACOp::IF_TRUE && isInteger(inst.arg1))
        {
            long long val = parseInteger(inst.arg1);
            if (val != 0)
            {
                // Condition is non-zero (true), branch is taken!
                inst.op = TACOp::GOTO;
                inst.arg1 = "";
                changed = true;
                newInsts.push_back(inst);
                unreachable = true;
                continue;
            }
            else
            {
                // Condition is 0, branch is never taken!
                changed = true;
                report.deadInstructionsRemoved++;
                continue;
            }
        }

        if (inst.op == TACOp::GOTO)
        {
            // Redundant jump elimination: GOTO L immediately followed by LABEL L
            if (i + 1 < prog.instructions.size() &&
                prog.instructions[i + 1].op == TACOp::LABEL &&
                prog.instructions[i + 1].result == inst.result)
            {
                changed = true;
                report.deadInstructionsRemoved++;
                continue;
            }
            newInsts.push_back(inst);
            unreachable = true;
            continue;
        }

        if (inst.op == TACOp::RETURN)
        {
            newInsts.push_back(inst);
            unreachable = true;
            continue;
        }

        newInsts.push_back(inst);
    }

    prog.instructions = newInsts;

    // 2. Dead temporary variable elimination
    // Count uses of temporaries
    std::unordered_map<std::string, int> useCount;
    for (const auto& inst : prog.instructions)
    {
        if (isTemp(inst.arg1))
            useCount[inst.arg1]++;
        if (isTemp(inst.arg2))
            useCount[inst.arg2]++;
    }

    std::vector<TACInstruction> finalInsts;
    for (const auto& inst : prog.instructions)
    {
        // If an instruction writes to a temporary tN and that temporary is never used, drop it.
        // Side-effecting operations like SERVE, SIP, GOTO, LABEL, RETURN are never dropped.
        if (isTemp(inst.result) &&
            inst.op != TACOp::SERVE_EXPR &&
            inst.op != TACOp::SERVE_STR &&
            inst.op != TACOp::SIP &&
            inst.op != TACOp::RETURN)
        {
            if (useCount[inst.result] == 0)
            {
                changed = true;
                report.deadInstructionsRemoved++;
                continue;
            }
        }
        finalInsts.push_back(inst);
    }

    prog.instructions = finalInsts;
    return changed;
}

TACProgram Optimizer::optimize(const TACProgram& inputProg)
{
    TACProgram currentProg = inputProg;
    report = OptimizationReport();

    // Iterate passes until fixpoint is reached (up to 10 iterations)
    const int maxIterations = 10;
    for (int iter = 0; iter < maxIterations; ++iter)
    {
        bool changed = false;

        changed |= passConstantPropagation(currentProg);
        changed |= passConstantFoldingAndAlgebraic(currentProg);
        changed |= passDeadCodeElimination(currentProg);

        if (!changed)
            break;
    }

    return currentProg;
}
