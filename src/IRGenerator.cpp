#include "IRGenerator.h"

IRGenerator::IRGenerator(const SymbolTable& table)
    : tempCount(0), labelCount(0), symbolTable(table)
{
}

std::string IRGenerator::newTemp()
{
    return "t" + std::to_string(tempCount++);
}

std::string IRGenerator::newLabel()
{
    return "L" + std::to_string(labelCount++);
}

TACProgram IRGenerator::generate(const std::shared_ptr<ProgramNode>& root)
{
    program.instructions.clear();
    tempCount = 0;
    labelCount = 0;

    if (!root)
        return program;

    for (const auto& stmt : root->statements)
    {
        generateStatement(stmt);
    }

    return program;
}

void IRGenerator::generateStatement(const ASTPtr& node)
{
    if (!node)
        return;

    if (auto decl = std::dynamic_pointer_cast<DeclarationNode>(node))
    {
        if (decl->initializer)
        {
            std::string val = generateExpression(decl->initializer);
            program.addInstruction(TACInstruction(TACOp::ASSIGN, decl->name, val));
        }
    }
    else if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(node))
    {
        std::string val = generateExpression(assign->expression);
        program.addInstruction(TACInstruction(TACOp::ASSIGN, assign->name, val));
    }
    else if (auto ifNode = std::dynamic_pointer_cast<IfNode>(node))
    {
        std::string cond = generateExpression(ifNode->condition);
        std::string lEnd = newLabel();

        if (ifNode->elseBranch)
        {
            std::string lElse = newLabel();
            program.addInstruction(TACInstruction(TACOp::IF_FALSE, lElse, cond));
            generateStatement(ifNode->thenBranch);
            program.addInstruction(TACInstruction(TACOp::GOTO, lEnd));
            program.addInstruction(TACInstruction(TACOp::LABEL, lElse));
            generateStatement(ifNode->elseBranch);
            program.addInstruction(TACInstruction(TACOp::LABEL, lEnd));
        }
        else
        {
            program.addInstruction(TACInstruction(TACOp::IF_FALSE, lEnd, cond));
            generateStatement(ifNode->thenBranch);
            program.addInstruction(TACInstruction(TACOp::LABEL, lEnd));
        }
    }
    else if (auto whileNode = std::dynamic_pointer_cast<WhileNode>(node))
    {
        std::string lStart = newLabel();
        std::string lEnd = newLabel();

        program.addInstruction(TACInstruction(TACOp::LABEL, lStart));
        std::string cond = generateExpression(whileNode->condition);
        program.addInstruction(TACInstruction(TACOp::IF_FALSE, lEnd, cond));
        generateStatement(whileNode->body);
        program.addInstruction(TACInstruction(TACOp::GOTO, lStart));
        program.addInstruction(TACInstruction(TACOp::LABEL, lEnd));
    }
    else if (auto blockNode = std::dynamic_pointer_cast<BlockNode>(node))
    {
        for (const auto& stmt : blockNode->statements)
        {
            generateStatement(stmt);
        }
    }
    else if (auto serveNode = std::dynamic_pointer_cast<ServeNode>(node))
    {
        auto lit = std::dynamic_pointer_cast<LiteralNode>(serveNode->expression);
        if (lit && lit->type == TokenType::STRING_LITERAL)
        {
            program.addInstruction(TACInstruction(TACOp::SERVE_STR, "", "", "", lit->value));
        }
        else
        {
            std::string val = generateExpression(serveNode->expression);
            program.addInstruction(TACInstruction(TACOp::SERVE_EXPR, "", val));
        }
    }
    else if (auto sipNode = std::dynamic_pointer_cast<SipNode>(node))
    {
        program.addInstruction(TACInstruction(TACOp::SIP, sipNode->name));
    }
    else if (auto retNode = std::dynamic_pointer_cast<ReturnNode>(node))
    {
        if (retNode->expression)
        {
            std::string val = generateExpression(retNode->expression);
            program.addInstruction(TACInstruction(TACOp::RETURN, "", val));
        }
        else
        {
            program.addInstruction(TACInstruction(TACOp::RETURN));
        }
    }
}

std::string IRGenerator::generateExpression(const ASTPtr& node)
{
    if (!node)
        return "";

    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(node))
    {
        if (lit->value == "hot")
            return "1";
        if (lit->value == "cold")
            return "0";
        return lit->value;
    }
    else if (auto id = std::dynamic_pointer_cast<IdentifierNode>(node))
    {
        return id->name;
    }
    else if (auto unary = std::dynamic_pointer_cast<UnaryExpressionNode>(node))
    {
        if (unary->op == "++")
        {
            auto idNode = std::dynamic_pointer_cast<IdentifierNode>(unary->operand);
            std::string idName = idNode ? idNode->name : generateExpression(unary->operand);
            std::string t = newTemp();
            program.addInstruction(TACInstruction(TACOp::ADD, t, idName, "1"));
            program.addInstruction(TACInstruction(TACOp::ASSIGN, idName, t));
            return idName;
        }
        else if (unary->op == "--")
        {
            auto idNode = std::dynamic_pointer_cast<IdentifierNode>(unary->operand);
            std::string idName = idNode ? idNode->name : generateExpression(unary->operand);
            std::string t = newTemp();
            program.addInstruction(TACInstruction(TACOp::SUB, t, idName, "1"));
            program.addInstruction(TACInstruction(TACOp::ASSIGN, idName, t));
            return idName;
        }
        else if (unary->op == "-")
        {
            std::string val = generateExpression(unary->operand);
            std::string t = newTemp();
            program.addInstruction(TACInstruction(TACOp::NEG, t, val));
            return t;
        }
        else if (unary->op == "!")
        {
            std::string val = generateExpression(unary->operand);
            std::string t = newTemp();
            program.addInstruction(TACInstruction(TACOp::NOT, t, val));
            return t;
        }
    }
    else if (auto bin = std::dynamic_pointer_cast<BinaryExpressionNode>(node))
    {
        std::string left = generateExpression(bin->left);
        std::string right = generateExpression(bin->right);
        std::string t = newTemp();

        TACOp op = TACOp::ADD;
        if (bin->op == "+") op = TACOp::ADD;
        else if (bin->op == "-") op = TACOp::SUB;
        else if (bin->op == "*") op = TACOp::MUL;
        else if (bin->op == "/") op = TACOp::DIV;
        else if (bin->op == "%") op = TACOp::MOD;
        else if (bin->op == "==") op = TACOp::EQ;
        else if (bin->op == "!=") op = TACOp::NE;
        else if (bin->op == "<") op = TACOp::LT;
        else if (bin->op == "<=") op = TACOp::LE;
        else if (bin->op == ">") op = TACOp::GT;
        else if (bin->op == ">=") op = TACOp::GE;
        else if (bin->op == "&&") op = TACOp::AND;
        else if (bin->op == "||") op = TACOp::OR;

        program.addInstruction(TACInstruction(op, t, left, right));
        return t;
    }

    return "";
}
