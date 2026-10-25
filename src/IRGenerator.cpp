#include "IRGenerator.h"

IRGenerator::IRGenerator(const SymbolTable& table)
    : tempCount(0), labelCount(0), symbolTable(table)
{
}

std::string IRGenerator::newTemp()
{
    return "$t" + std::to_string(tempCount++);
}

std::string IRGenerator::newLabel()
{
    return "L" + std::to_string(labelCount++);
}

std::string IRGenerator::declareVar(const std::string& name)
{
    int n = declCount[name]++;
    std::string unique = (n == 0) ? name : name + "." + std::to_string(n);
    scopes.back()[name] = unique;
    return unique;
}

std::string IRGenerator::resolveVar(const std::string& name) const
{
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        auto f = it->find(name);
        if (f != it->end())
            return f->second;
    }
    return name;
}

void IRGenerator::unsupported(const std::string& what)
{
    errors.push_back(what);
}

TACProgram IRGenerator::generate(const std::shared_ptr<ProgramNode>& root)
{
    program.instructions.clear();
    tempCount = 0;
    labelCount = 0;
    scopes.clear();
    declCount.clear();
    errors.clear();
    scopes.push_back({});

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
        std::string val;
        if (decl->initializer)
            val = generateExpression(decl->initializer);

        std::string unique = declareVar(decl->name);

        if (decl->initializer)
            program.addInstruction(TACInstruction(TACOp::ASSIGN, unique, val));
    }
    else if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(node))
    {
        std::string val = generateExpression(assign->expression);
        program.addInstruction(TACInstruction(TACOp::ASSIGN, resolveVar(assign->name), val));
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
        scopes.push_back({});
        for (const auto& stmt : blockNode->statements)
        {
            generateStatement(stmt);
        }
        scopes.pop_back();
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
        program.addInstruction(TACInstruction(TACOp::SIP, resolveVar(sipNode->name)));
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
        if (lit->type == TokenType::CHAR_LITERAL)
            return std::to_string(lit->value.empty() ? 0 : (int)(unsigned char)lit->value[0]);
        if (lit->type == TokenType::FLOAT_LITERAL)
        {
            unsupported("float values are not supported by the x86 backend");
            return "0";
        }
        if (lit->type == TokenType::STRING_LITERAL)
        {
            unsupported("string variables/values are not supported by the x86 backend; only serve(\"literal\") is");
            return "0";
        }
        return lit->value;
    }
    else if (auto id = std::dynamic_pointer_cast<IdentifierNode>(node))
    {
        return resolveVar(id->name);
    }
    else if (auto unary = std::dynamic_pointer_cast<UnaryExpressionNode>(node))
    {
        if (unary->op == "++")
        {
            auto idNode = std::dynamic_pointer_cast<IdentifierNode>(unary->operand);
            std::string idName = idNode ? resolveVar(idNode->name) : generateExpression(unary->operand);
            std::string t = newTemp();
            program.addInstruction(TACInstruction(TACOp::ADD, t, idName, "1"));
            program.addInstruction(TACInstruction(TACOp::ASSIGN, idName, t));
            return idName;
        }
        else if (unary->op == "--")
        {
            auto idNode = std::dynamic_pointer_cast<IdentifierNode>(unary->operand);
            std::string idName = idNode ? resolveVar(idNode->name) : generateExpression(unary->operand);
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
        // Short-circuit evaluation for && and ||
        if (bin->op == "&&" || bin->op == "||")
        {
            std::string t = newTemp();
            std::string lEnd = newLabel();

            std::string left = generateExpression(bin->left);
            program.addInstruction(TACInstruction(TACOp::NE, t, left, "0"));
            program.addInstruction(TACInstruction(
                bin->op == "&&" ? TACOp::IF_FALSE : TACOp::IF_TRUE, lEnd, t));

            std::string right = generateExpression(bin->right);
            program.addInstruction(TACInstruction(TACOp::NE, t, right, "0"));
            program.addInstruction(TACInstruction(TACOp::LABEL, lEnd));
            return t;
        }

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
