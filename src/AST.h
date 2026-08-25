#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

// Helper for print indentation
inline void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
}

// Program node: list of statements (the root of the AST)
class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Program\n";
        for (const auto& stmt : statements) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

// Block node: a list of statements enclosed in curly braces {}
class Block : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Block\n";
        for (const auto& stmt : statements) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

// Declaration node: type identifier [= expression]
class Declaration : public ASTNode {
public:
    std::string type;
    std::string identifier;
    std::unique_ptr<ASTNode> initializer; // Can be nullptr

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Declaration (" << type << " " << identifier << ")\n";
        if (initializer) {
            printIndent(indent + 1);
            std::cout << "Initializer:\n";
            initializer->print(indent + 2);
        }
    }
};

// Assignment node: identifier = expression
class Assignment : public ASTNode {
public:
    std::string identifier;
    std::unique_ptr<ASTNode> expression;

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Assignment (" << identifier << " =)\n";
        if (expression) {
            expression->print(indent + 1);
        }
    }
};

// BinaryExpression node: lhs op rhs
class BinaryExpression : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> lhs;
    std::unique_ptr<ASTNode> rhs;

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "BinaryExpression (" << op << ")\n";
        if (lhs) lhs->print(indent + 1);
        if (rhs) rhs->print(indent + 1);
    }
};

// UnaryExpression node: op operand (prefix/postfix)
class UnaryExpression : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> operand;
    bool isPostfix;

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "UnaryExpression (" << op << ", " << (isPostfix ? "postfix" : "prefix") << ")\n";
        if (operand) operand->print(indent + 1);
    }
};

// If node: if (condition) thenBranch else elseBranch
class If : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> thenBranch;
    std::unique_ptr<ASTNode> elseBranch; // Can be nullptr

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "If\n";

        printIndent(indent + 1);
        std::cout << "Condition:\n";
        if (condition) condition->print(indent + 2);

        printIndent(indent + 1);
        std::cout << "Then:\n";
        if (thenBranch) thenBranch->print(indent + 2);

        if (elseBranch) {
            printIndent(indent + 1);
            std::cout << "Else:\n";
            elseBranch->print(indent + 2);
        }
    }
};

// While node: while (condition) body
class While : public ASTNode {
public:
    std::unique_ptr<ASTNode> condition;
    std::unique_ptr<ASTNode> body;

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "While\n";

        printIndent(indent + 1);
        std::cout << "Condition:\n";
        if (condition) condition->print(indent + 2);

        printIndent(indent + 1);
        std::cout << "Body:\n";
        if (body) body->print(indent + 2);
    }
};

// Print node: print(expression)
class Print : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression;

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Print\n";
        if (expression) expression->print(indent + 1);
    }
};

// Return node: return expression
class Return : public ASTNode {
public:
    std::unique_ptr<ASTNode> expression; // Can be nullptr

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Return\n";
        if (expression) {
            expression->print(indent + 1);
        } else {
            printIndent(indent + 1);
            std::cout << "void\n";
        }
    }
};

// Function node: function declaration (e.g. int main())
class Function : public ASTNode {
public:
    std::string type;
    std::string identifier;
    std::vector<std::pair<std::string, std::string>> parameters; // Pair of {type, name}
    std::unique_ptr<ASTNode> body; // Should be a Block

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Function (" << type << " " << identifier << ") Params: ";
        if (parameters.empty()) {
            std::cout << "none";
        } else {
            for (size_t i = 0; i < parameters.size(); ++i) {
                if (i > 0) std::cout << ", ";
                std::cout << parameters[i].first << " " << parameters[i].second;
            }
        }
        std::cout << "\n";
        if (body) body->print(indent + 1);
    }
};

// Literal node: constants like 10, 3.14, 'a', true, "hello"
class Literal : public ASTNode {
public:
    std::string value;
    std::string type; // "INTEGER", "FLOAT", "CHAR", "BOOLEAN", "STRING"

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Literal (" << type << "): " << value << "\n";
    }
};

// Identifier node: variable references like a, b, c
class Identifier : public ASTNode {
public:
    std::string name;

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Identifier: " << name << "\n";
    }
};

#endif // AST_H
