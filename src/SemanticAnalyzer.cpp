#include "SemanticAnalyzer.h"

#include <cctype>
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer(SymbolTable& table)
    : symbolTable(table), ownsSymbolTable(false)
{
}

SemanticAnalyzer::SemanticAnalyzer()
    : symbolTable(*(new SymbolTable())), ownsSymbolTable(true)
{
}

SemanticAnalyzer::~SemanticAnalyzer()
{
    if (ownsSymbolTable)
    {
        delete &symbolTable;
    }
}

void SemanticAnalyzer::reportError(const std::string& message, int line)
{
    std::string err = "Semantic Error: " + message;
    if (line > 0)
    {
        err += " at line " + std::to_string(line);
    }
    errors.push_back(err);
}

bool SemanticAnalyzer::analyze(const std::shared_ptr<ProgramNode>& program, bool printOutput)
{
    errors.clear();
    symbolTable.clear();

    if (!program)
    {
        reportError("Invalid or empty AST for semantic analysis", 0);
        if (printOutput)
            printReport();
        return false;
    }

    analyzeProgram(program);

    if (printOutput)
        printReport();
    return errors.empty();
}

void SemanticAnalyzer::printReport() const
{
    if (errors.empty())
    {
        std::cout << "\nSemantic Analysis PASSED\n";
    }
    else
    {
        std::cout << "\n";
        for (const auto& err : errors)
        {
            std::cout << err << "\n";
        }
        std::cout << "\nSemantic Analysis FAILED — " << errors.size() << " semantic errors found.\n";
    }
}

const std::vector<std::string>& SemanticAnalyzer::getErrors() const
{
    return errors;
}

bool SemanticAnalyzer::hasErrors() const
{
    return !errors.empty();
}

void SemanticAnalyzer::analyzeNode(const ASTPtr& node)
{
    if (!node)
        return;

    if (auto program = std::dynamic_pointer_cast<ProgramNode>(node))
        analyzeProgram(program);
    else if (auto block = std::dynamic_pointer_cast<BlockNode>(node))
        analyzeBlock(block);
    else if (auto decl = std::dynamic_pointer_cast<DeclarationNode>(node))
        analyzeDeclaration(decl);
    else if (auto assign = std::dynamic_pointer_cast<AssignmentNode>(node))
        analyzeAssignment(assign);
    else if (auto ifNode = std::dynamic_pointer_cast<IfNode>(node))
        analyzeIf(ifNode);
    else if (auto whileNode = std::dynamic_pointer_cast<WhileNode>(node))
        analyzeWhile(whileNode);
    else if (auto serveNode = std::dynamic_pointer_cast<ServeNode>(node))
        analyzeServe(serveNode);
    else if (auto sipNode = std::dynamic_pointer_cast<SipNode>(node))
        analyzeSip(sipNode);
    else if (auto returnNode = std::dynamic_pointer_cast<ReturnNode>(node))
        analyzeReturn(returnNode);
}

void SemanticAnalyzer::analyzeProgram(const std::shared_ptr<ProgramNode>& node)
{
    for (const auto& stmt : node->statements)
    {
        analyzeNode(stmt);
    }
}

void SemanticAnalyzer::analyzeBlock(const std::shared_ptr<BlockNode>& node)
{
    symbolTable.enterScope();

    for (const auto& stmt : node->statements)
    {
        analyzeNode(stmt);
    }

    symbolTable.exitScope();
}

void SemanticAnalyzer::analyzeDeclaration(const std::shared_ptr<DeclarationNode>& node)
{
    // Check for variable redeclaration in current scope
    if (symbolTable.containsInCurrentScope(node->name))
    {
        reportError("Variable '" + node->name + "' is already declared in current scope", node->line);
    }

    std::string varType = node->type;
    std::string initType = "";

    if (node->initializer)
    {
        initType = analyzeExpression(node->initializer);
    }

    if (!varType.empty())
    {
        // Explicit type declared: e.g. brew int x = 10;
        if (!initType.empty() && initType != "error")
        {
            if (!areTypesCompatible(varType, initType))
            {
                reportError("Type mismatch in declaration of '" + node->name + "'. Cannot assign '" + initType + "' to '" + varType + "'", node->line);
            }
        }
    }
    else
    {
        // Untyped brew declaration: e.g. brew marks = 78;
        if (!initType.empty() && initType != "error")
        {
            varType = initType;
        }
        else
        {
            varType = "int"; // default type
        }
    }

    // Register symbol into current scope
    symbolTable.addSymbol(node->name, "variable", node->line, varType, symbolTable.getCurrentScopeLevel());
}

void SemanticAnalyzer::analyzeAssignment(const std::shared_ptr<AssignmentNode>& node)
{
    std::string exprType = "";
    if (node->expression)
    {
        exprType = analyzeExpression(node->expression);
    }

    // Check if target variable is declared
    const Symbol* sym = symbolTable.getSymbol(node->name);
    if (!sym)
    {
        reportError("Variable '" + node->name + "' is not declared", node->line);
        return;
    }

    // Check assignment type compatibility
    if (!exprType.empty() && exprType != "error")
    {
        if (!areTypesCompatible(sym->type, exprType))
        {
            reportError("Type mismatch in assignment to '" + node->name + "'. Cannot assign '" + exprType + "' to '" + sym->type + "'", node->line);
        }
    }
}

void SemanticAnalyzer::analyzeIf(const std::shared_ptr<IfNode>& node)
{
    if (node->condition)
    {
        std::string condType = analyzeExpression(node->condition);
        if (condType != "error" && !isBooleanOrNumericType(condType))
        {
            reportError("Condition in 'if' statement must be boolean or numeric", node->line);
        }
    }

    if (node->thenBranch)
    {
        analyzeNode(node->thenBranch);
    }

    if (node->elseBranch)
    {
        analyzeNode(node->elseBranch);
    }
}

void SemanticAnalyzer::analyzeWhile(const std::shared_ptr<WhileNode>& node)
{
    if (node->condition)
    {
        std::string condType = analyzeExpression(node->condition);
        if (condType != "error" && !isBooleanOrNumericType(condType))
        {
            reportError("Condition in 'while' statement must be boolean or numeric", node->line);
        }
    }

    if (node->body)
    {
        analyzeNode(node->body);
    }
}

void SemanticAnalyzer::analyzeServe(const std::shared_ptr<ServeNode>& node)
{
    if (node->expression)
    {
        analyzeExpression(node->expression);
    }
}

void SemanticAnalyzer::analyzeSip(const std::shared_ptr<SipNode>& node)
{
    const Symbol* sym = symbolTable.getSymbol(node->name);
    if (!sym)
    {
        reportError("Variable '" + node->name + "' is not declared", node->line);
    }
}

void SemanticAnalyzer::analyzeReturn(const std::shared_ptr<ReturnNode>& node)
{
    if (node->expression)
    {
        analyzeExpression(node->expression);
    }
}

std::string SemanticAnalyzer::analyzeExpression(const ASTPtr& expr)
{
    if (!expr)
        return "void";

    if (auto lit = std::dynamic_pointer_cast<LiteralNode>(expr))
        return analyzeLiteral(lit);
    else if (auto id = std::dynamic_pointer_cast<IdentifierNode>(expr))
        return analyzeIdentifier(id);
    else if (auto bin = std::dynamic_pointer_cast<BinaryExpressionNode>(expr))
        return analyzeBinaryExpr(bin);
    else if (auto un = std::dynamic_pointer_cast<UnaryExpressionNode>(expr))
        return analyzeUnaryExpr(un);

    return "unknown";
}

std::string SemanticAnalyzer::analyzeLiteral(const std::shared_ptr<LiteralNode>& node)
{
    if (!node->literalType.empty() && node->literalType != "unknown")
    {
        return node->literalType;
    }

    return deduceLiteralType(node->value);
}

std::string SemanticAnalyzer::deduceLiteralType(const std::string& value) const
{
    if (value == "hot" || value == "cold")
        return "bool";
    if (value.size() >= 2 && value.front() == '"' && value.back() == '"')
        return "string";
    if (value.size() >= 2 && value.front() == '\'' && value.back() == '\'')
        return "char";
    if (value.find('.') != std::string::npos)
        return "float";

    bool isNum = true;
    for (char c : value)
    {
        if (!isdigit(c) && c != '-')
        {
            isNum = false;
            break;
        }
    }
    if (isNum && !value.empty())
        return "int";

    return "string";
}

std::string SemanticAnalyzer::analyzeIdentifier(const std::shared_ptr<IdentifierNode>& node)
{
    const Symbol* sym = symbolTable.getSymbol(node->name);
    if (!sym)
    {
        reportError("Variable '" + node->name + "' is not declared", node->line);
        return "error";
    }

    return sym->type;
}

std::string SemanticAnalyzer::analyzeBinaryExpr(const std::shared_ptr<BinaryExpressionNode>& node)
{
    std::string leftType = analyzeExpression(node->left);
    std::string rightType = analyzeExpression(node->right);

    if (leftType == "error" || rightType == "error")
    {
        return "error";
    }

    const std::string& op = node->op;
    int line = node->line > 0 ? node->line : (node->left ? node->left->line : 0);

    // Arithmetic operators: +, -, *, /, %
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%")
    {
        if (op == "+")
        {
            if (leftType == "string" && rightType == "string")
                return "string";

            if (isNumericType(leftType) && isNumericType(rightType))
            {
                if (leftType == "float" || rightType == "float")
                    return "float";
                return "int";
            }

            reportError("Invalid operand types for operator '+'", line);
            return "error";
        }
        else if (op == "%")
        {
            if (leftType == "int" && rightType == "int")
                return "int";

            reportError("Modulo operator '%' requires integer operands", line);
            return "error";
        }
        else // -, *, /
        {
            if (isNumericType(leftType) && isNumericType(rightType))
            {
                if (leftType == "float" || rightType == "float")
                    return "float";
                return "int";
            }

            reportError("Invalid operand types for operator '" + op + "'", line);
            return "error";
        }
    }

    // Relational operators: <, >, <=, >=
    if (op == "<" || op == ">" || op == "<=" || op == ">=")
    {
        if ((isNumericType(leftType) && isNumericType(rightType)) ||
            (leftType == "char" && rightType == "char"))
        {
            return "bool";
        }

        reportError("Relational operator '" + op + "' requires numeric operands", line);
        return "error";
    }

    // Equality operators: ==, !=
    if (op == "==" || op == "!=")
    {
        if (areTypesCompatible(leftType, rightType) || areTypesCompatible(rightType, leftType))
        {
            return "bool";
        }

        reportError("Equality operator '" + op + "' cannot compare '" + leftType + "' and '" + rightType + "'", line);
        return "error";
    }

    // Logical operators: &&, ||
    if (op == "&&" || op == "||")
    {
        if (isBooleanOrNumericType(leftType) && isBooleanOrNumericType(rightType))
        {
            return "bool";
        }

        reportError("Logical operator '" + op + "' requires boolean operands", line);
        return "error";
    }

    return "unknown";
}

std::string SemanticAnalyzer::analyzeUnaryExpr(const std::shared_ptr<UnaryExpressionNode>& node)
{
    std::string operandType = analyzeExpression(node->operand);
    if (operandType == "error")
        return "error";

    const std::string& op = node->op;
    int line = node->line > 0 ? node->line : (node->operand ? node->operand->line : 0);

    if (op == "!")
    {
        if (isBooleanOrNumericType(operandType))
            return "bool";

        reportError("Logical NOT operator '!' requires boolean or numeric operand", line);
        return "error";
    }

    if (op == "-")
    {
        if (isNumericType(operandType))
            return operandType;

        reportError("Unary '-' operator requires numeric operand", line);
        return "error";
    }

    if (op == "++" || op == "--")
    {
        auto id = std::dynamic_pointer_cast<IdentifierNode>(node->operand);
        if (!id)
        {
            reportError("Operand of '" + op + "' must be a variable", line);
            return "error";
        }

        if (!isNumericType(operandType))
        {
            reportError("Operand of '" + op + "' must be numeric", line);
            return "error";
        }

        return operandType;
    }

    return operandType;
}

bool SemanticAnalyzer::areTypesCompatible(const std::string& targetType, const std::string& exprType) const
{
    if (targetType == exprType)
        return true;

    if (targetType == "unknown" || exprType == "unknown")
        return true;

    // Promotion of int to float
    if (targetType == "float" && exprType == "int")
        return true;

    return false;
}

bool SemanticAnalyzer::isNumericType(const std::string& type) const
{
    return (type == "int" || type == "float");
}

bool SemanticAnalyzer::isBooleanOrNumericType(const std::string& type) const
{
    return (type == "bool" || type == "int" || type == "float");
}
