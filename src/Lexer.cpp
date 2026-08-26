#include "Lexer.h"
#include <cctype>
#include <iostream>
#include <unordered_map>

Lexer::Lexer(const std::string& sourceCode)
    : source(sourceCode), position(0), line(1), column(1) {
}

char Lexer::currentChar() {
    if (position >= source.length()) {
        return '\0';
    }

    return source[position];
}

void Lexer::advance() {
    if (currentChar() == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }

    position++;
}

void Lexer::skipWhitespace() {
    while (currentChar() != '\0' &&
           std::isspace(static_cast<unsigned char>(currentChar()))) {
        advance();
    }
}

void Lexer::skipComment() {

    // Single-line comment
    if (currentChar() == '/' &&
        position + 1 < source.length() &&
        source[position + 1] == '/') {

        while (currentChar() != '\0' && currentChar() != '\n') {
            advance();
        }

        return;
    }

    // Multi-line comment
    if (currentChar() == '/' &&
        position + 1 < source.length() &&
        source[position + 1] == '*') {

        advance();
        advance();

        while (currentChar() != '\0') {

            if (currentChar() == '*' &&
                position + 1 < source.length() &&
                source[position + 1] == '/') {

                advance();
                advance();
                return;
            }

            advance();
        }

        std::cerr << "Lexical Error: Unterminated comment\n";
    }
}

Token Lexer::scanIdentifierOrKeyword() {

    int startLine = line;
    int startColumn = column;

    std::string lexeme;

    while (std::isalnum(static_cast<unsigned char>(currentChar())) ||
           currentChar() == '_') {

        lexeme += currentChar();
        advance();
    }

    static const std::unordered_map<std::string, TokenType> keywords = {

        {"brew", TokenType::BREW},
        {"serve", TokenType::SERVE},
        {"sip", TokenType::SIP},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"return", TokenType::RETURN},

        {"hot", TokenType::HOT},
        {"cold", TokenType::COLD},

        {"int", TokenType::INT_TYPE},
        {"float", TokenType::FLOAT_TYPE},
        {"char", TokenType::CHAR_TYPE},
        {"bool", TokenType::BOOL_TYPE},
        {"string", TokenType::STRING_TYPE}
    };

    auto it = keywords.find(lexeme);

    if (it != keywords.end()) {

        if (lexeme == "hot" || lexeme == "cold") {
            return Token(
                TokenType::BOOLEAN_LITERAL,
                lexeme,
                startLine,
                startColumn
            );
        }

        return Token(
            it->second,
            lexeme,
            startLine,
            startColumn
        );
    }

    return Token(
        TokenType::IDENTIFIER,
        lexeme,
        startLine,
        startColumn
    );
}

Token Lexer::scanNumber() {

    int startLine = line;
    int startColumn = column;

    std::string lexeme;
    bool hasDecimal = false;

    while (std::isdigit(static_cast<unsigned char>(currentChar()))) {

        lexeme += currentChar();
        advance();
    }

    if (currentChar() == '.') {

        hasDecimal = true;

        lexeme += currentChar();
        advance();

        while (std::isdigit(static_cast<unsigned char>(currentChar()))) {

            lexeme += currentChar();
            advance();
        }
    }

    if (hasDecimal) {

        return Token(
            TokenType::FLOAT_LITERAL,
            lexeme,
            startLine,
            startColumn
        );

    }

    return Token(
        TokenType::INTEGER_LITERAL,
        lexeme,
        startLine,
        startColumn
    );
}

Token Lexer::scanString() {

    int startLine = line;
    int startColumn = column;

    std::string lexeme;

    // Skip opening "
    advance();

    while (currentChar() != '"' && currentChar() != '\0') {

        if (currentChar() == '\n') {

            std::cerr
                << "Lexical Error at line "
                << line
                << ": Unterminated string\n";

            break;
        }

        lexeme += currentChar();
        advance();
    }

    if (currentChar() == '"') {
        advance();
    }

    return Token(
        TokenType::STRING_LITERAL,
        lexeme,
        startLine,
        startColumn
    );
}

Token Lexer::scanCharacter() {

    int startLine = line;
    int startColumn = column;

    std::string lexeme;

    // Skip opening '
    advance();

    if (currentChar() != '\0' &&
        currentChar() != '\'' &&
        currentChar() != '\n') {

        lexeme += currentChar();
        advance();

    } else {

        std::cerr
            << "Lexical Error at line "
            << line
            << ", column "
            << column
            << ": Invalid character literal\n";
    }

    if (currentChar() == '\'') {
        advance();
    }

    return Token(
        TokenType::CHAR_LITERAL,
        lexeme,
        startLine,
        startColumn
    );
}

Token Lexer::scanOperatorOrDelimiter() {

    int startLine = line;
    int startColumn = column;

    char c = currentChar();
    char next = '\0';

    if (position + 1 < source.length()) {
        next = source[position + 1];
    }

    // Two-character operators

    if (c == '=' && next == '=') {

        advance();
        advance();

        return Token(
            TokenType::EQUAL,
            "==",
            startLine,
            startColumn
        );
    }

    if (c == '!' && next == '=') {

        advance();
        advance();

        return Token(
            TokenType::NOT_EQUAL,
            "!=",
            startLine,
            startColumn
        );
    }

    if (c == '<' && next == '=') {

        advance();
        advance();

        return Token(
            TokenType::LESS_EQUAL,
            "<=",
            startLine,
            startColumn
        );
    }

    if (c == '>' && next == '=') {

        advance();
        advance();

        return Token(
            TokenType::GREATER_EQUAL,
            ">=",
            startLine,
            startColumn
        );
    }

    if (c == '&' && next == '&') {

        advance();
        advance();

        return Token(
            TokenType::AND,
            "&&",
            startLine,
            startColumn
        );
    }

    if (c == '|' && next == '|') {

        advance();
        advance();

        return Token(
            TokenType::OR,
            "||",
            startLine,
            startColumn
        );
    }

    if (c == '+' && next == '+') {

        advance();
        advance();

        return Token(
            TokenType::INCREMENT,
            "++",
            startLine,
            startColumn
        );
    }

    if (c == '-' && next == '-') {

        advance();
        advance();

        return Token(
            TokenType::DECREMENT,
            "--",
            startLine,
            startColumn
        );
    }

    // Single-character tokens

    advance();

    switch (c) {

        case '+':
            return Token(TokenType::PLUS, "+", startLine, startColumn);

        case '-':
            return Token(TokenType::MINUS, "-", startLine, startColumn);

        case '*':
            return Token(TokenType::MULTIPLY, "*", startLine, startColumn);

        case '/':
            return Token(TokenType::DIVIDE, "/", startLine, startColumn);

        case '%':
            return Token(TokenType::MODULO, "%", startLine, startColumn);

        case '=':
            return Token(TokenType::ASSIGN, "=", startLine, startColumn);

        case '<':
            return Token(TokenType::LESS, "<", startLine, startColumn);

        case '>':
            return Token(TokenType::GREATER, ">", startLine, startColumn);

        case '!':
            return Token(TokenType::NOT, "!", startLine, startColumn);

        case '(':
            return Token(TokenType::LEFT_PAREN, "(", startLine, startColumn);

        case ')':
            return Token(TokenType::RIGHT_PAREN, ")", startLine, startColumn);

        case '{':
            return Token(TokenType::LEFT_BRACE, "{", startLine, startColumn);

        case '}':
            return Token(TokenType::RIGHT_BRACE, "}", startLine, startColumn);

        case ';':
            return Token(TokenType::SEMICOLON, ";", startLine, startColumn);

        case ',':
            return Token(TokenType::COMMA, ",", startLine, startColumn);

        default:
            return Token(
                TokenType::INVALID,
                std::string(1, c),
                startLine,
                startColumn
            );
    }
}

std::vector<Token> Lexer::tokenize() {

    std::vector<Token> tokens;

    while (currentChar() != '\0') {

        skipWhitespace();

        if (currentChar() == '\0') {
            break;
        }

        // Comments
        if (currentChar() == '/' &&
            position + 1 < source.length() &&
            (source[position + 1] == '/' ||
             source[position + 1] == '*')) {

            skipComment();
            continue;
        }

        // Identifier / keyword
        if (std::isalpha(static_cast<unsigned char>(currentChar())) ||
            currentChar() == '_') {

            tokens.push_back(scanIdentifierOrKeyword());
            continue;
        }

        // Number
        if (std::isdigit(static_cast<unsigned char>(currentChar()))) {

            tokens.push_back(scanNumber());
            continue;
        }

        // String
        if (currentChar() == '"') {

            tokens.push_back(scanString());
            continue;
        }

        // Character
        if (currentChar() == '\'') {

            tokens.push_back(scanCharacter());
            continue;
        }

        // Operator / delimiter
        tokens.push_back(scanOperatorOrDelimiter());
    }

    tokens.emplace_back(
        TokenType::END_OF_FILE,
        "EOF",
        line,
        column
    );

    return tokens;
}

std::string tokenTypeToString(TokenType type) {

    switch (type) {

        case TokenType::BREW: return "BREW";
        case TokenType::SERVE: return "SERVE";
        case TokenType::SIP: return "SIP";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::RETURN: return "RETURN";

        case TokenType::HOT:
        case TokenType::COLD:
        case TokenType::BOOLEAN_LITERAL:
            return "BOOLEAN_LITERAL";

        case TokenType::INT_TYPE: return "INT_TYPE";
        case TokenType::FLOAT_TYPE: return "FLOAT_TYPE";
        case TokenType::CHAR_TYPE: return "CHAR_TYPE";
        case TokenType::BOOL_TYPE: return "BOOL_TYPE";
        case TokenType::STRING_TYPE: return "STRING_TYPE";

        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INTEGER_LITERAL: return "INTEGER_LITERAL";
        case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";

        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::MODULO: return "MODULO";

        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::NOT_EQUAL: return "NOT_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::GREATER: return "GREATER";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";

        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::NOT: return "NOT";

        case TokenType::INCREMENT: return "INCREMENT";
        case TokenType::DECREMENT: return "DECREMENT";

        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";

        case TokenType::END_OF_FILE: return "EOF";
        case TokenType::INVALID: return "INVALID";
    }

    return "UNKNOWN";
}