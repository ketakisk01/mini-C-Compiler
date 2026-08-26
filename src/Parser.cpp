#include "Parser.h"
#include <iostream>

Parser::Parser(
    const std::vector<Token>& tokenList,
    SymbolTable& table
)
    : tokens(tokenList),
      current(0),
      symbolTable(table) {
}

Token& Parser::peek() {
    return tokens[current];
}

Token& Parser::previous() {
    return tokens[current - 1];
}

bool Parser::check(TokenType type) {

    return peek().type == type;
}

bool Parser::match(TokenType type) {

    if (check(type)) {

        current++;

        return true;
    }

    return false;
}

Token Parser::consume(
    TokenType type,
    const std::string& message
) {

    if (check(type)) {

        return tokens[current++];
    }

    syntaxError(message);

    return Token(
        TokenType::INVALID,
        "",
        peek().line,
        peek().column
    );
}

void Parser::syntaxError(
    const std::string& message
) {

    std::cerr
        << "Syntax Error at line "
        << peek().line
        << ", column "
        << peek().column
        << ": "
        << message
        << "\n";
}

std::shared_ptr<ProgramNode> Parser::parse() {

    auto program = std::make_shared<ProgramNode>();

    while (!check(TokenType::END_OF_FILE)) {

        program->statements.push_back(
            parseStatement()
        );
    }

    return program;
}

ASTPtr Parser::parseStatement() {

    if (check(TokenType::BREW)) {
        return parseDeclaration();
    }

    if (check(TokenType::IF)) {
        return parseIf();
    }

    if (check(TokenType::WHILE)) {
        return parseWhile();
    }

    if (check(TokenType::SERVE)) {
        return parseServe();
    }

    if (check(TokenType::SIP)) {
        return parseSip();
    }

    if (check(TokenType::RETURN)) {
        return parseReturn();
    }

    if (check(TokenType::LEFT_BRACE)) {
        return parseBlock();
    }

    if (check(TokenType::IDENTIFIER)) {
        return parseAssignment();
    }

    syntaxError(
        "Unexpected token '" +
        peek().lexeme +
        "'"
    );

    current++;

    return nullptr;
}

ASTPtr Parser::parseDeclaration() {

    consume(
        TokenType::BREW,
        "Expected 'brew'"
    );

    // Optional data type
    if (check(TokenType::INT_TYPE) ||
        check(TokenType::FLOAT_TYPE) ||
        check(TokenType::CHAR_TYPE) ||
        check(TokenType::BOOL_TYPE) ||
        check(TokenType::STRING_TYPE)) {

        current++;
    }

    Token name = consume(
        TokenType::IDENTIFIER,
        "Expected variable name after 'brew'"
    );

    symbolTable.addSymbol(
        name.lexeme,
        "variable",
        name.line
    );

    ASTPtr initializer = nullptr;

    if (match(TokenType::ASSIGN)) {

        initializer = parseExpression();
    }

    consume(
        TokenType::SEMICOLON,
        "Expected ';' after declaration"
    );

    return std::make_shared<DeclarationNode>(
        name.lexeme,
        initializer
    );
}

ASTPtr Parser::parseAssignment() {

    Token name = consume(
        TokenType::IDENTIFIER,
        "Expected identifier"
    );

    consume(
        TokenType::ASSIGN,
        "Expected '=' after identifier"
    );

    ASTPtr expression = parseExpression();

    consume(
        TokenType::SEMICOLON,
        "Expected ';' after assignment"
    );

    return std::make_shared<AssignmentNode>(
        name.lexeme,
        expression
    );
}

ASTPtr Parser::parseIf() {

    consume(
        TokenType::IF,
        "Expected 'if'"
    );

    consume(
        TokenType::LEFT_PAREN,
        "Expected '(' after 'if'"
    );

    ASTPtr condition = parseExpression();

    consume(
        TokenType::RIGHT_PAREN,
        "Expected ')' after condition"
    );

    ASTPtr thenBranch = parseStatement();

    ASTPtr elseBranch = nullptr;

    if (match(TokenType::ELSE)) {

        elseBranch = parseStatement();
    }

    return std::make_shared<IfNode>(
        condition,
        thenBranch,
        elseBranch
    );
}

ASTPtr Parser::parseWhile() {

    consume(
        TokenType::WHILE,
        "Expected 'while'"
    );

    consume(
        TokenType::LEFT_PAREN,
        "Expected '(' after 'while'"
    );

    ASTPtr condition = parseExpression();

    consume(
        TokenType::RIGHT_PAREN,
        "Expected ')' after condition"
    );

    ASTPtr body = parseStatement();

    return std::make_shared<WhileNode>(
        condition,
        body
    );
}

ASTPtr Parser::parseServe() {

    consume(
        TokenType::SERVE,
        "Expected 'serve'"
    );

    consume(
        TokenType::LEFT_PAREN,
        "Expected '(' after 'serve'"
    );

    ASTPtr expression = parseExpression();

    consume(
        TokenType::RIGHT_PAREN,
        "Expected ')' after serve expression"
    );

    consume(
        TokenType::SEMICOLON,
        "Expected ';' after serve"
    );

    return std::make_shared<ServeNode>(
        expression
    );
}

ASTPtr Parser::parseSip() {

    consume(
        TokenType::SIP,
        "Expected 'sip'"
    );

    consume(
        TokenType::LEFT_PAREN,
        "Expected '(' after 'sip'"
    );

    Token name = consume(
        TokenType::IDENTIFIER,
        "Expected identifier inside sip()"
    );

    consume(
        TokenType::RIGHT_PAREN,
        "Expected ')' after sip identifier"
    );

    consume(
        TokenType::SEMICOLON,
        "Expected ';' after sip"
    );

    return std::make_shared<SipNode>(
        name.lexeme
    );
}

ASTPtr Parser::parseReturn() {

    consume(
        TokenType::RETURN,
        "Expected 'return'"
    );

    ASTPtr expression = nullptr;

    if (!check(TokenType::SEMICOLON)) {

        expression = parseExpression();
    }

    consume(
        TokenType::SEMICOLON,
        "Expected ';' after return"
    );

    return std::make_shared<ReturnNode>(
        expression
    );
}

ASTPtr Parser::parseBlock() {

    consume(
        TokenType::LEFT_BRACE,
        "Expected '{'"
    );

    auto block = std::make_shared<BlockNode>();

    while (!check(TokenType::RIGHT_BRACE) &&
           !check(TokenType::END_OF_FILE)) {

        block->statements.push_back(
            parseStatement()
        );
    }

    consume(
        TokenType::RIGHT_BRACE,
        "Expected '}' after block"
    );

    return block;
}

ASTPtr Parser::parseExpression() {

    return parseLogicalOr();
}

ASTPtr Parser::parseLogicalOr() {

    ASTPtr expression = parseLogicalAnd();

    while (match(TokenType::OR)) {

        Token op = previous();

        ASTPtr right = parseLogicalAnd();

        expression =
            std::make_shared<BinaryExpressionNode>(
                op.lexeme,
                expression,
                right
            );
    }

    return expression;
}

ASTPtr Parser::parseLogicalAnd() {

    ASTPtr expression = parseEquality();

    while (match(TokenType::AND)) {

        Token op = previous();

        ASTPtr right = parseEquality();

        expression =
            std::make_shared<BinaryExpressionNode>(
                op.lexeme,
                expression,
                right
            );
    }

    return expression;
}

ASTPtr Parser::parseEquality() {

    ASTPtr expression = parseComparison();

    while (
        match(TokenType::EQUAL) ||
        match(TokenType::NOT_EQUAL)
    ) {

        Token op = previous();

        ASTPtr right = parseComparison();

        expression =
            std::make_shared<BinaryExpressionNode>(
                op.lexeme,
                expression,
                right
            );
    }

    return expression;
}

ASTPtr Parser::parseComparison() {

    ASTPtr expression = parseTerm();

    while (
        match(TokenType::LESS) ||
        match(TokenType::GREATER) ||
        match(TokenType::LESS_EQUAL) ||
        match(TokenType::GREATER_EQUAL)
    ) {

        Token op = previous();

        ASTPtr right = parseTerm();

        expression =
            std::make_shared<BinaryExpressionNode>(
                op.lexeme,
                expression,
                right
            );
    }

    return expression;
}

ASTPtr Parser::parseTerm() {

    ASTPtr expression = parseFactor();

    while (
        match(TokenType::PLUS) ||
        match(TokenType::MINUS)
    ) {

        Token op = previous();

        ASTPtr right = parseFactor();

        expression =
            std::make_shared<BinaryExpressionNode>(
                op.lexeme,
                expression,
                right
            );
    }

    return expression;
}

ASTPtr Parser::parseFactor() {

    ASTPtr expression = parseUnary();

    while (
        match(TokenType::MULTIPLY) ||
        match(TokenType::DIVIDE) ||
        match(TokenType::MODULO)
    ) {

        Token op = previous();

        ASTPtr right = parseUnary();

        expression =
            std::make_shared<BinaryExpressionNode>(
                op.lexeme,
                expression,
                right
            );
    }

    return expression;
}

ASTPtr Parser::parseUnary() {

    if (
        match(TokenType::NOT) ||
        match(TokenType::MINUS) ||
        match(TokenType::INCREMENT) ||
        match(TokenType::DECREMENT)
    ) {

        Token op = previous();

        ASTPtr operand = parseUnary();

        return std::make_shared<UnaryExpressionNode>(
            op.lexeme,
            operand
        );
    }

    return parsePrimary();
}

ASTPtr Parser::parsePrimary() {

    if (
        match(TokenType::INTEGER_LITERAL) ||
        match(TokenType::FLOAT_LITERAL) ||
        match(TokenType::CHAR_LITERAL) ||
        match(TokenType::STRING_LITERAL) ||
        match(TokenType::BOOLEAN_LITERAL)
    ) {

        return std::make_shared<LiteralNode>(
            previous().lexeme
        );
    }

    if (match(TokenType::IDENTIFIER)) {

        return std::make_shared<IdentifierNode>(
            previous().lexeme
        );
    }

    if (match(TokenType::LEFT_PAREN)) {

        ASTPtr expression = parseExpression();

        consume(
            TokenType::RIGHT_PAREN,
            "Expected ')' after expression"
        );

        return expression;
    }

    syntaxError(
        "Expected expression"
    );

    current++;

    return std::make_shared<LiteralNode>(
        "ERROR"
    );
}