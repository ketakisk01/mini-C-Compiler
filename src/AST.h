#ifndef AST_H
#define AST_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

inline void printIndent(int indent)
{
    for (int i = 0; i < indent; i++)
        std::cout << "  ";
}

class ASTNode
{
public:
    int line = 0;

    virtual ~ASTNode() = default;

    virtual void print(int indent = 0) const = 0;
};

using ASTPtr = std::shared_ptr<ASTNode>;

class ProgramNode : public ASTNode
{
public:
    std::vector<ASTPtr> statements;

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Program\n";

        for (const auto& statement : statements)
        {
            if (statement)
                statement->print(indent + 1);
        }
    }
};

class BlockNode : public ASTNode
{
public:
    std::vector<ASTPtr> statements;

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Block\n";

        for (const auto& statement : statements)
        {
            if (statement)
                statement->print(indent + 1);
        }
    }
};

class LiteralNode : public ASTNode
{
public:
    std::string value;
    std::string literalType;

    LiteralNode(const std::string& value, const std::string& literalType = "")
        : value(value), literalType(literalType)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Literal: " << value << "\n";
    }
};

class IdentifierNode : public ASTNode
{
public:
    std::string name;

    IdentifierNode(const std::string& name)
        : name(name)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Identifier: " << name << "\n";
    }
};

class DeclarationNode : public ASTNode
{
public:
    std::string type;
    std::string name;
    ASTPtr initializer;

    DeclarationNode(
        const std::string& name,
        ASTPtr initializer,
        const std::string& type = ""
    )
        : type(type),
          name(name),
          initializer(initializer)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Declaration: ";
        if (!type.empty())
            std::cout << type << " ";
        std::cout << name << "\n";

        if (initializer)
            initializer->print(indent + 1);
    }
};

class AssignmentNode : public ASTNode
{
public:
    std::string name;
    ASTPtr expression;

    AssignmentNode(
        const std::string& name,
        ASTPtr expression
    )
        : name(name),
          expression(expression)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Assignment: " << name << "\n";

        if (expression)
            expression->print(indent + 1);
    }
};

class BinaryExpressionNode : public ASTNode
{
public:
    std::string op;
    ASTPtr left;
    ASTPtr right;

    BinaryExpressionNode(
        const std::string& op,
        ASTPtr left,
        ASTPtr right
    )
        : op(op),
          left(left),
          right(right)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Binary Operator: " << op << "\n";

        if (left)
            left->print(indent + 1);

        if (right)
            right->print(indent + 1);
    }
};

class UnaryExpressionNode : public ASTNode
{
public:
    std::string op;
    ASTPtr operand;

    UnaryExpressionNode(
        const std::string& op,
        ASTPtr operand
    )
        : op(op),
          operand(operand)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Unary Operator: " << op << "\n";

        if (operand)
            operand->print(indent + 1);
    }
};

class IfNode : public ASTNode
{
public:
    ASTPtr condition;
    ASTPtr thenBranch;
    ASTPtr elseBranch;

    IfNode(
        ASTPtr condition,
        ASTPtr thenBranch,
        ASTPtr elseBranch
    )
        : condition(condition),
          thenBranch(thenBranch),
          elseBranch(elseBranch)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "If\n";

        printIndent(indent + 1);
        std::cout << "Condition\n";

        if (condition)
            condition->print(indent + 2);

        printIndent(indent + 1);
        std::cout << "Then\n";

        if (thenBranch)
            thenBranch->print(indent + 2);

        if (elseBranch)
        {
            printIndent(indent + 1);
            std::cout << "Else\n";
            elseBranch->print(indent + 2);
        }
    }
};

class WhileNode : public ASTNode
{
public:
    ASTPtr condition;
    ASTPtr body;

    WhileNode(
        ASTPtr condition,
        ASTPtr body
    )
        : condition(condition),
          body(body)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "While\n";

        if (condition)
            condition->print(indent + 1);

        if (body)
            body->print(indent + 1);
    }
};

class ServeNode : public ASTNode
{
public:
    ASTPtr expression;

    ServeNode(ASTPtr expression)
        : expression(expression)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Serve\n";

        if (expression)
            expression->print(indent + 1);
    }
};

class SipNode : public ASTNode
{
public:
    std::string name;

    SipNode(const std::string& name)
        : name(name)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Sip: " << name << "\n";
    }
};

class ReturnNode : public ASTNode
{
public:
    ASTPtr expression;

    ReturnNode(ASTPtr expression)
        : expression(expression)
    {
    }

    void print(int indent = 0) const override
    {
        printIndent(indent);
        std::cout << "Return\n";

        if (expression)
            expression->print(indent + 1);
    }
};

#endif