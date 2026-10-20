#include "TAC.h"
#include <iomanip>

std::string tacOpToString(TACOp op)
{
    switch (op)
    {
        case TACOp::ADD: return "+";
        case TACOp::SUB: return "-";
        case TACOp::MUL: return "*";
        case TACOp::DIV: return "/";
        case TACOp::MOD: return "%";
        case TACOp::EQ: return "==";
        case TACOp::NE: return "!=";
        case TACOp::LT: return "<";
        case TACOp::GT: return ">";
        case TACOp::LE: return "<=";
        case TACOp::GE: return ">=";
        case TACOp::AND: return "&&";
        case TACOp::OR: return "||";
        case TACOp::NOT: return "!";
        case TACOp::NEG: return "-";
        case TACOp::ASSIGN: return "=";
        case TACOp::LABEL: return "LABEL";
        case TACOp::GOTO: return "GOTO";
        case TACOp::IF_TRUE: return "IF_TRUE";
        case TACOp::IF_FALSE: return "IF_FALSE";
        case TACOp::SERVE_STR: return "SERVE_STR";
        case TACOp::SERVE_EXPR: return "SERVE_EXPR";
        case TACOp::SIP: return "SIP";
        case TACOp::RETURN: return "RETURN";
    }
    return "UNKNOWN";
}

std::string TACInstruction::toString() const
{
    switch (op)
    {
        case TACOp::LABEL:
            return result + ":";
        case TACOp::GOTO:
            return "goto " + result;
        case TACOp::IF_TRUE:
            return "if " + arg1 + " goto " + result;
        case TACOp::IF_FALSE:
            return "ifFalse " + arg1 + " goto " + result;
        case TACOp::ASSIGN:
            return result + " = " + arg1;
        case TACOp::ADD:
            return result + " = " + arg1 + " + " + arg2;
        case TACOp::SUB:
            return result + " = " + arg1 + " - " + arg2;
        case TACOp::MUL:
            return result + " = " + arg1 + " * " + arg2;
        case TACOp::DIV:
            return result + " = " + arg1 + " / " + arg2;
        case TACOp::MOD:
            return result + " = " + arg1 + " % " + arg2;
        case TACOp::EQ:
            return result + " = " + arg1 + " == " + arg2;
        case TACOp::NE:
            return result + " = " + arg1 + " != " + arg2;
        case TACOp::LT:
            return result + " = " + arg1 + " < " + arg2;
        case TACOp::GT:
            return result + " = " + arg1 + " > " + arg2;
        case TACOp::LE:
            return result + " = " + arg1 + " <= " + arg2;
        case TACOp::GE:
            return result + " = " + arg1 + " >= " + arg2;
        case TACOp::AND:
            return result + " = " + arg1 + " && " + arg2;
        case TACOp::OR:
            return result + " = " + arg1 + " || " + arg2;
        case TACOp::NOT:
            return result + " = ! " + arg1;
        case TACOp::NEG:
            return result + " = - " + arg1;
        case TACOp::SERVE_STR:
            return "serve \"" + strVal + "\"";
        case TACOp::SERVE_EXPR:
            return "serve " + arg1;
        case TACOp::SIP:
            return "sip " + result;
        case TACOp::RETURN:
            if (arg1.empty()) return "return";
            return "return " + arg1;
    }
    return "";
}

void TACProgram::print(std::ostream& os) const
{
    int index = 0;
    for (const auto& inst : instructions)
    {
        if (inst.op == TACOp::LABEL)
        {
            os << inst.toString() << "\n";
        }
        else
        {
            os << "    " << inst.toString() << "\n";
        }
        index++;
    }
}
