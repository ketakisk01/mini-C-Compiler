#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType
{
    // Keywords
    BREW,
    SERVE,
    SIP,
    IF,
    ELSE,
    WHILE,
    RETURN,
    HOT,
    COLD,

    // Data types
    INT_TYPE,
    FLOAT_TYPE,
    CHAR_TYPE,
    BOOL_TYPE,
    STRING_TYPE,

    // Identifiers and literals
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    CHAR_LITERAL,
    STRING_LITERAL,
    BOOLEAN_LITERAL,

    // Operators
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    MODULO,
    ASSIGN,
    EQUAL,
    NOT_EQUAL,
    LESS,
    GREATER,
    LESS_EQUAL,
    GREATER_EQUAL,
    AND,
    OR,
    NOT,
    INCREMENT,
    DECREMENT,

    // Delimiters
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    SEMICOLON,
    COMMA,

    // Special
    END_OF_FILE,
    INVALID
};

struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(
        TokenType t,
        const std::string& l,
        int ln,
        int col
    )
        : type(t), lexeme(l), line(ln), column(col)
    {
    }
};

std::string tokenTypeToString(TokenType type);

#endif