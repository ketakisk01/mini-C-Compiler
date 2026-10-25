#include "CodeGenerator.h"
#include <sstream>
#include <cctype>

CodeGenerator::CodeGenerator()
    : nextOffset(0), stringCount(0)
{
}

bool CodeGenerator::isInteger(const std::string& str) const
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

int CodeGenerator::allocateSlot(const std::string& name)
{
    if (stackOffsets.count(name))
        return stackOffsets[name];

    nextOffset += 4;
    stackOffsets[name] = nextOffset;
    return nextOffset;
}

std::string CodeGenerator::getOperand(const std::string& name)
{
    if (isInteger(name))
    {
        return "$" + name;
    }

    int offset = allocateSlot(name);
    return "-" + std::to_string(offset) + "(%ebp)";
}

std::string CodeGenerator::generateAssembly(const TACProgram& prog, const std::string& filename)
{
    stackOffsets.clear();
    stringLabels.clear();
    nextOffset = 0;
    stringCount = 0;

    std::ostringstream rodata;
    std::ostringstream text;

    bool hasExplicitReturn = false;
    for (const auto& inst : prog.instructions)
    {
        if (inst.op == TACOp::RETURN)
        {
            hasExplicitReturn = true;
            break;
        }
    }

    // 1. Identify string literals
    for (const auto& inst : prog.instructions)
    {
        if (inst.op == TACOp::SERVE_STR)
        {
            if (!stringLabels.count(inst.strVal))
            {
                std::string label = "LC" + std::to_string(stringCount++);
                stringLabels[inst.strVal] = label;
            }
        }
    }

    // 2. Pre-allocate stack slots for all variables and temporaries
    for (const auto& inst : prog.instructions)
    {
        if (!inst.result.empty() && inst.op != TACOp::LABEL && inst.op != TACOp::GOTO &&
            inst.op != TACOp::IF_TRUE && inst.op != TACOp::IF_FALSE)
        {
            allocateSlot(inst.result);
        }
        if (!inst.arg1.empty() && !isInteger(inst.arg1))
        {
            allocateSlot(inst.arg1);
        }
        if (!inst.arg2.empty() && !isInteger(inst.arg2))
        {
            allocateSlot(inst.arg2);
        }
    }

    // Stack alignment to 16-byte boundary
    int frameSize = ((nextOffset + 15) / 16) * 16;
    if (frameSize < 16) frameSize = 16;

    // Header and Read-only Data Section
    rodata << "    .file   \"" << filename << "\"\n";
    rodata << "    .def    _printf;  .scl    2;  .type   32; .endef\n";
    rodata << "    .def    _puts;    .scl    2;  .type   32; .endef\n";
    rodata << "    .def    _scanf;   .scl    2;  .type   32; .endef\n";
    rodata << "    .section .rdata,\"dr\"\n";
    rodata << "LC_fmt_int:\n";
    rodata << "    .ascii  \"%d\\12\\0\"\n";
    rodata << "LC_fmt_in:\n";
    rodata << "    .ascii  \"%d\\0\"\n";

    for (const auto& pair : stringLabels)
    {
        rodata << pair.second << ":\n";
        rodata << "    .ascii  \"" << pair.first << "\\0\"\n";
    }

    // Text Section and Function Prologue
    text << "    .text\n";
    text << "    .globl  _main\n";
    text << "    .def    _main;  .scl    2;  .type   32; .endef\n";
    text << "_main:\n";
    text << "    pushl   %ebp\n";
    text << "    movl    %esp, %ebp\n";
    text << "    subl    $" << frameSize << ", %esp\n";

    // Translate instructions
    for (const auto& inst : prog.instructions)
    {
        switch (inst.op)
        {
            case TACOp::LABEL:
                text << inst.result << ":\n";
                break;

            case TACOp::GOTO:
                text << "    jmp     " << inst.result << "\n";
                break;

            case TACOp::IF_TRUE:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    cmpl    $0, %eax\n";
                text << "    jne     " << inst.result << "\n";
                break;

            case TACOp::IF_FALSE:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    cmpl    $0, %eax\n";
                text << "    je      " << inst.result << "\n";
                break;

            case TACOp::ASSIGN:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::ADD:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    addl    " << getOperand(inst.arg2) << ", %eax\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::SUB:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    subl    " << getOperand(inst.arg2) << ", %eax\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::MUL:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    imull   " << getOperand(inst.arg2) << ", %eax\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::DIV:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    cdq\n";
                text << "    movl    " << getOperand(inst.arg2) << ", %ecx\n";
                text << "    idivl   %ecx\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::MOD:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    cdq\n";
                text << "    movl    " << getOperand(inst.arg2) << ", %ecx\n";
                text << "    idivl   %ecx\n";
                text << "    movl    %edx, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::EQ:
            case TACOp::NE:
            case TACOp::LT:
            case TACOp::GT:
            case TACOp::LE:
            case TACOp::GE:
            {
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    movl    " << getOperand(inst.arg2) << ", %edx\n";
                text << "    cmpl    %edx, %eax\n";
                std::string setcc;
                if (inst.op == TACOp::EQ) setcc = "sete";
                else if (inst.op == TACOp::NE) setcc = "setne";
                else if (inst.op == TACOp::LT) setcc = "setl";
                else if (inst.op == TACOp::LE) setcc = "setle";
                else if (inst.op == TACOp::GT) setcc = "setg";
                else if (inst.op == TACOp::GE) setcc = "setge";
                text << "    " << setcc << "    %al\n";
                text << "    movzbl  %al, %eax\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;
            }

            case TACOp::AND:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    cmpl    $0, %eax\n";
                text << "    setne   %al\n";
                text << "    movzbl  %al, %eax\n";
                text << "    movl    " << getOperand(inst.arg2) << ", %edx\n";
                text << "    cmpl    $0, %edx\n";
                text << "    setne   %dl\n";
                text << "    movzbl  %dl, %edx\n";
                text << "    andl    %edx, %eax\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::OR:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    cmpl    $0, %eax\n";
                text << "    setne   %al\n";
                text << "    movzbl  %al, %eax\n";
                text << "    movl    " << getOperand(inst.arg2) << ", %edx\n";
                text << "    cmpl    $0, %edx\n";
                text << "    setne   %dl\n";
                text << "    movzbl  %dl, %edx\n";
                text << "    orl     %edx, %eax\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::NOT:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    cmpl    $0, %eax\n";
                text << "    sete    %al\n";
                text << "    movzbl  %al, %eax\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::NEG:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    negl    %eax\n";
                text << "    movl    %eax, " << getOperand(inst.result) << "\n";
                break;

            case TACOp::SERVE_STR:
                text << "    pushl   $" << stringLabels[inst.strVal] << "\n";
                text << "    call    _puts\n";
                text << "    addl    $4, %esp\n";
                break;

            case TACOp::SERVE_EXPR:
                text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                text << "    pushl   %eax\n";
                text << "    pushl   $LC_fmt_int\n";
                text << "    call    _printf\n";
                text << "    addl    $8, %esp\n";
                break;

            case TACOp::SIP:
                text << "    leal    " << getOperand(inst.result) << ", %eax\n";
                text << "    pushl   %eax\n";
                text << "    pushl   $LC_fmt_in\n";
                text << "    call    _scanf\n";
                text << "    addl    $8, %esp\n";
                break;

            case TACOp::RETURN:
                if (!inst.arg1.empty())
                {
                    text << "    movl    " << getOperand(inst.arg1) << ", %eax\n";
                }
                else
                {
                    text << "    xorl    %eax, %eax\n";
                }
                text << "    leave\n";
                text << "    ret\n";
                break;
        }
    }

    if (!hasExplicitReturn)
    {
        text << "    xorl    %eax, %eax\n";
        text << "    leave\n";
        text << "    ret\n";
    }

    return rodata.str() + "\n" + text.str();
}
