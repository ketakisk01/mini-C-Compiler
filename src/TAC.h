#ifndef TAC_H
#define TAC_H

#include <string>
#include <vector>
#include <iostream>

enum class TACOp
{
    // Arithmetic
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,

    // Relational / Comparison
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,

    // Logical
    AND,
    OR,
    NOT,

    // Unary
    NEG,

    // Copy / Assignment
    ASSIGN,

    // Control Flow
    LABEL,
    GOTO,
    IF_TRUE,
    IF_FALSE,

    // I/O & Functions
    SERVE_STR,
    SERVE_EXPR,
    SIP,
    RETURN
};

std::string tacOpToString(TACOp op);

struct TACInstruction
{
    TACOp op;
    std::string result;
    std::string arg1;
    std::string arg2;
    std::string strVal; // For string literals in SERVE_STR

    TACInstruction(TACOp op, const std::string& res = "", const std::string& a1 = "", const std::string& a2 = "", const std::string& str = "")
        : op(op), result(res), arg1(a1), arg2(a2), strVal(str)
    {
    }

    std::string toString() const;
};

class TACProgram
{
public:
    std::vector<TACInstruction> instructions;

    void addInstruction(const TACInstruction& inst)
    {
        instructions.push_back(inst);
    }

    void print(std::ostream& os = std::cout) const;
};

#endif // TAC_H
