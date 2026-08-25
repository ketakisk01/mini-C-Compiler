#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    INTEGER,
    FLOAT,
    CHAR,
    BOOLEAN,
    OPERATOR,
    DELIMITER,
    STRING,
    EOF_TOKEN, // Use EOF_TOKEN to avoid conflicts with standard C EOF macro
    INVALID
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KEYWORD: return "KEYWORD";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::FLOAT: return "FLOAT";
        case TokenType::CHAR: return "CHAR";
        case TokenType::BOOLEAN: return "BOOLEAN";
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::DELIMITER: return "DELIMITER";
        case TokenType::STRING: return "STRING";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType type, std::string lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
};

#endif // TOKEN_H
