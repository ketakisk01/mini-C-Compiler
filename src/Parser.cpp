#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens, SymbolTable& symbolTable)
    : tokens(tokens), current(0), symbolTable(symbolTable) {}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EOF_TOKEN;
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchKeyword(const std::string& keyword) {
    if (check(TokenType::KEYWORD) && peek().lexeme == keyword) {
        advance();
        return true;
    }
    return false;
}

bool Parser::checkKeyword(const std::string& keyword) const {
    if (isAtEnd()) return false;
    return peek().type == TokenType::KEYWORD && peek().lexeme == keyword;
}

Token Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) return advance();
    error(peek(), errorMessage);
    throw std::runtime_error(errorMessage);
}

Token Parser::consumeKeyword(const std::string& keyword, const std::string& errorMessage) {
    if (check(TokenType::KEYWORD) && peek().lexeme == keyword) return advance();
    error(peek(), errorMessage);
    throw std::runtime_error(errorMessage);
}

void Parser::error(const Token& token, const std::string& message) {
    std::string errMsg = "[Syntax Error] Line " + std::to_string(token.line) +
                         ", Column " + std::to_string(token.column) +
                         ": " + message + " (got '" + token.lexeme + "')";
    errors.push_back(errMsg);
}

void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::DELIMITER && previous().lexeme == ";") return;

        if (peek().type == TokenType::KEYWORD) {
            std::string lex = peek().lexeme;
            if (lex == "if" || lex == "while" || lex == "print" || lex == "return" ||
                lex == "int" || lex == "float" || lex == "char" || lex == "bool") {
                return;
            }
        }
        advance();
    }
}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
        } catch (const std::exception& e) {
            synchronize();
        }
    }
    return program;
}

bool Parser::hasErrors() const {
    return !errors.empty();
}

void Parser::printErrors() const {
    for (const auto& err : errors) {
        std::cout << err << "\n";
    }
}

std::unique_ptr<ASTNode> Parser::parseStatement() {
    if (match(TokenType::DELIMITER) && previous().lexeme == "{") {
        return parseBlock();
    }
    if (check(TokenType::KEYWORD)) {
        std::string lex = peek().lexeme;
        if (lex == "int" || lex == "float" || lex == "char" || lex == "bool") {
            return parseDeclaration();
        } else if (lex == "if") {
            return parseIf();
        } else if (lex == "while") {
            return parseWhile();
        } else if (lex == "print") {
            return parsePrint();
        } else if (lex == "return") {
            return parseReturn();
        }
    }
    return parseAssignmentOrExprStmt();
}

std::unique_ptr<ASTNode> Parser::parseBlock() {
    symbolTable.enterScope();
    auto block = std::make_unique<Block>();
    while (!check(TokenType::DELIMITER) || peek().lexeme != "}") {
        if (isAtEnd()) {
            error(peek(), "Unterminated block, expected '}'");
            symbolTable.exitScope();
            throw std::runtime_error("Unterminated block");
        }
        try {
            auto stmt = parseStatement();
            if (stmt) {
                block->statements.push_back(std::move(stmt));
            }
        } catch (const std::exception& e) {
            synchronize();
        }
    }
    consume(TokenType::DELIMITER, "Expected '}' after block");
    symbolTable.exitScope();
    return block;
}

std::unique_ptr<ASTNode> Parser::parseDeclaration() {
    Token typeToken = advance(); // Consume type keyword
    Token nameToken = consume(TokenType::IDENTIFIER, "Expected identifier after type");

    if (match(TokenType::DELIMITER) && previous().lexeme == "(") {
        // It's a function declaration!
        std::vector<std::pair<std::string, std::string>> params;
        if (!check(TokenType::DELIMITER) || peek().lexeme != ")") {
            do {
                Token pType = consume(TokenType::KEYWORD, "Expected parameter type");
                Token pName = consume(TokenType::IDENTIFIER, "Expected parameter identifier");
                params.push_back({pType.lexeme, pName.lexeme});
            } while (match(TokenType::DELIMITER) && previous().lexeme == ",");
        }
        consume(TokenType::DELIMITER, "Expected ')' after parameters");

        // Now parse function body (which is a block statement)
        consume(TokenType::DELIMITER, "Expected '{' to start function body");

        symbolTable.enterScope();
        // Insert parameters into inner scope
        for (const auto& p : params) {
            symbolTable.insert(p.second, p.first, nameToken.line, nameToken.column);
        }

        auto body = std::make_unique<Block>();
        while (!check(TokenType::DELIMITER) || peek().lexeme != "}") {
            if (isAtEnd()) {
                error(peek(), "Unterminated function block, expected '}'");
                symbolTable.exitScope();
                throw std::runtime_error("Unterminated function block");
            }
            try {
                auto stmt = parseStatement();
                if (stmt) {
                    body->statements.push_back(std::move(stmt));
                }
            } catch (const std::exception& e) {
                synchronize();
            }
        }
        consume(TokenType::DELIMITER, "Expected '}' after function body");
        symbolTable.exitScope();

        // Register the function itself in the outer scope
        if (!symbolTable.insert(nameToken.lexeme, typeToken.lexeme + " function", nameToken.line, nameToken.column)) {
            error(nameToken, "Redeclaration of identifier '" + nameToken.lexeme + "' in the same scope");
        }

        auto funcNode = std::make_unique<Function>();
        funcNode->type = typeToken.lexeme;
        funcNode->identifier = nameToken.lexeme;
        funcNode->parameters = params;
        funcNode->body = std::move(body);
        return funcNode;
    }

    std::unique_ptr<ASTNode> initializer = nullptr;
    if (match(TokenType::OPERATOR) && previous().lexeme == "=") {
        initializer = parseExpression();
    }

    consume(TokenType::DELIMITER, "Expected ';' after variable declaration");

    // Insert identifier to Symbol Table
    if (!symbolTable.insert(nameToken.lexeme, typeToken.lexeme, nameToken.line, nameToken.column)) {
        error(nameToken, "Redeclaration of identifier '" + nameToken.lexeme + "' in the same scope");
    }

    auto decl = std::make_unique<Declaration>();
    decl->type = typeToken.lexeme;
    decl->identifier = nameToken.lexeme;
    decl->initializer = std::move(initializer);
    return decl;
}

std::unique_ptr<ASTNode> Parser::parseAssignmentOrExprStmt() {
    // LL(2) check: identifier followed by assignment operator '='
    if (check(TokenType::IDENTIFIER) && current + 1 < tokens.size() &&
        tokens[current + 1].type == TokenType::OPERATOR && tokens[current + 1].lexeme == "=") {
        Token nameToken = advance(); // Consume identifier
        advance(); // Consume '='
        auto expr = parseExpression();
        consume(TokenType::DELIMITER, "Expected ';' after assignment");

        // Validate identifier exists in SymbolTable
        Symbol sym;
        if (!symbolTable.lookup(nameToken.lexeme, sym)) {
            error(nameToken, "Use of undeclared identifier '" + nameToken.lexeme + "'");
        }

        auto assign = std::make_unique<Assignment>();
        assign->identifier = nameToken.lexeme;
        assign->expression = std::move(expr);
        return assign;
    }

    // Otherwise, parse standard expression statement (e.g. x++;)
    auto expr = parseExpression();
    consume(TokenType::DELIMITER, "Expected ';' after expression statement");
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseIf() {
    consumeKeyword("if", "Expected 'if'");
    consume(TokenType::DELIMITER, "Expected '(' after 'if'");
    auto condition = parseExpression();
    consume(TokenType::DELIMITER, "Expected ')' after if condition");

    auto thenBranch = parseStatement();

    std::unique_ptr<ASTNode> elseBranch = nullptr;
    if (matchKeyword("else")) {
        elseBranch = parseStatement();
    }

    auto ifNode = std::make_unique<If>();
    ifNode->condition = std::move(condition);
    ifNode->thenBranch = std::move(thenBranch);
    ifNode->elseBranch = std::move(elseBranch);
    return ifNode;
}

std::unique_ptr<ASTNode> Parser::parseWhile() {
    consumeKeyword("while", "Expected 'while'");
    consume(TokenType::DELIMITER, "Expected '(' after 'while'");
    auto condition = parseExpression();
    consume(TokenType::DELIMITER, "Expected ')' after while condition");

    auto body = parseStatement();

    auto whileNode = std::make_unique<While>();
    whileNode->condition = std::move(condition);
    whileNode->body = std::move(body);
    return whileNode;
}

std::unique_ptr<ASTNode> Parser::parsePrint() {
    consumeKeyword("print", "Expected 'print'");
    consume(TokenType::DELIMITER, "Expected '(' after 'print'");
    auto expr = parseExpression();
    consume(TokenType::DELIMITER, "Expected ')' after print argument");
    consume(TokenType::DELIMITER, "Expected ';' after print statement");

    auto printNode = std::make_unique<Print>();
    printNode->expression = std::move(expr);
    return printNode;
}

std::unique_ptr<ASTNode> Parser::parseReturn() {
    consumeKeyword("return", "Expected 'return'");
    std::unique_ptr<ASTNode> expr = nullptr;
    if (!check(TokenType::DELIMITER) || peek().lexeme != ";") {
        expr = parseExpression();
    }
    consume(TokenType::DELIMITER, "Expected ';' after return statement");

    auto returnNode = std::make_unique<Return>();
    returnNode->expression = std::move(expr);
    return returnNode;
}

std::unique_ptr<ASTNode> Parser::parseExpression() {
    return parseLogicalOr();
}

std::unique_ptr<ASTNode> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (check(TokenType::OPERATOR) && peek().lexeme == "||") {
        advance(); // consume "||"
        std::string op = "||";
        auto rhs = parseLogicalAnd();
        auto binary = std::make_unique<BinaryExpression>();
        binary->op = op;
        binary->lhs = std::move(expr);
        binary->rhs = std::move(rhs);
        expr = std::move(binary);
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    while (check(TokenType::OPERATOR) && peek().lexeme == "&&") {
        advance(); // consume "&&"
        std::string op = "&&";
        auto rhs = parseEquality();
        auto binary = std::make_unique<BinaryExpression>();
        binary->op = op;
        binary->lhs = std::move(expr);
        binary->rhs = std::move(rhs);
        expr = std::move(binary);
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseEquality() {
    auto expr = parseRelational();
    while (check(TokenType::OPERATOR) && (peek().lexeme == "==" || peek().lexeme == "!=")) {
        Token opToken = advance(); // consume operator
        std::string op = opToken.lexeme;
        auto rhs = parseRelational();
        auto binary = std::make_unique<BinaryExpression>();
        binary->op = op;
        binary->lhs = std::move(expr);
        binary->rhs = std::move(rhs);
        expr = std::move(binary);
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseRelational() {
    auto expr = parseAdditive();
    while (check(TokenType::OPERATOR) && (peek().lexeme == "<" || peek().lexeme == ">" ||
                                          peek().lexeme == "<=" || peek().lexeme == ">=")) {
        Token opToken = advance(); // consume operator
        std::string op = opToken.lexeme;
        auto rhs = parseAdditive();
        auto binary = std::make_unique<BinaryExpression>();
        binary->op = op;
        binary->lhs = std::move(expr);
        binary->rhs = std::move(rhs);
        expr = std::move(binary);
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseAdditive() {
    auto expr = parseMultiplicative();
    while (check(TokenType::OPERATOR) && (peek().lexeme == "+" || peek().lexeme == "-")) {
        Token opToken = advance(); // consume operator
        std::string op = opToken.lexeme;
        auto rhs = parseMultiplicative();
        auto binary = std::make_unique<BinaryExpression>();
        binary->op = op;
        binary->lhs = std::move(expr);
        binary->rhs = std::move(rhs);
        expr = std::move(binary);
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseMultiplicative() {
    auto expr = parseUnary();
    while (check(TokenType::OPERATOR) && (peek().lexeme == "*" || peek().lexeme == "/" || peek().lexeme == "%")) {
        Token opToken = advance(); // consume operator
        std::string op = opToken.lexeme;
        auto rhs = parseUnary();
        auto binary = std::make_unique<BinaryExpression>();
        binary->op = op;
        binary->lhs = std::move(expr);
        binary->rhs = std::move(rhs);
        expr = std::move(binary);
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parseUnary() {
    if (check(TokenType::OPERATOR) && (peek().lexeme == "!" || peek().lexeme == "-" ||
                                        peek().lexeme == "++" || peek().lexeme == "--")) {
        Token opToken = advance(); // consume operator
        std::string op = opToken.lexeme;
        auto operand = parseUnary();
        auto unary = std::make_unique<UnaryExpression>();
        unary->op = op;
        unary->operand = std::move(operand);
        unary->isPostfix = false;
        return unary;
    }
    return parsePostfix();
}

std::unique_ptr<ASTNode> Parser::parsePostfix() {
    auto expr = parsePrimary();
    if (check(TokenType::OPERATOR) && (peek().lexeme == "++" || peek().lexeme == "--")) {
        Token opToken = advance(); // consume operator
        std::string op = opToken.lexeme;
        auto unary = std::make_unique<UnaryExpression>();
        unary->op = op;
        unary->operand = std::move(expr);
        unary->isPostfix = true;
        return unary;
    }
    return expr;
}

std::unique_ptr<ASTNode> Parser::parsePrimary() {
    if (match(TokenType::INTEGER)) {
        auto lit = std::make_unique<Literal>();
        lit->value = previous().lexeme;
        lit->type = "INTEGER";
        return lit;
    }
    if (match(TokenType::FLOAT)) {
        auto lit = std::make_unique<Literal>();
        lit->value = previous().lexeme;
        lit->type = "FLOAT";
        return lit;
    }
    if (match(TokenType::CHAR)) {
        auto lit = std::make_unique<Literal>();
        lit->value = previous().lexeme;
        lit->type = "CHAR";
        return lit;
    }
    if (match(TokenType::BOOLEAN)) {
        auto lit = std::make_unique<Literal>();
        lit->value = previous().lexeme;
        lit->type = "BOOLEAN";
        return lit;
    }
    if (match(TokenType::STRING)) {
        auto lit = std::make_unique<Literal>();
        lit->value = previous().lexeme;
        lit->type = "STRING";
        return lit;
    }
    if (match(TokenType::IDENTIFIER)) {
        std::string name = previous().lexeme;
        Symbol sym;
        if (!symbolTable.lookup(name, sym)) {
            error(previous(), "Use of undeclared identifier '" + name + "'");
        }
        auto id = std::make_unique<Identifier>();
        id->name = name;
        return id;
    }
    if (match(TokenType::DELIMITER) && previous().lexeme == "(") {
        auto expr = parseExpression();
        consume(TokenType::DELIMITER, "Expected ')' after expression");
        return expr;
    }

    error(peek(), "Expected expression");
    throw std::runtime_error("Expected expression");
}
