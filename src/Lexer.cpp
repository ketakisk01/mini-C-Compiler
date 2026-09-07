#include "Lexer.h"

#include <unordered_map>
#include <cctype>

static const std::unordered_map<std::string, TokenType> keywords =
{
    {"brew", TokenType::BREW},
    {"serve", TokenType::SERVE},
    {"sip", TokenType::SIP},
    {"if", TokenType::IF},
    {"else", TokenType::ELSE},
    {"while", TokenType::WHILE},
    {"return", TokenType::RETURN},
    {"hot", TokenType::BOOLEAN_LITERAL},
    {"cold", TokenType::BOOLEAN_LITERAL},

    {"int", TokenType::INT_TYPE},
    {"float", TokenType::FLOAT_TYPE},
    {"char", TokenType::CHAR_TYPE},
    {"bool", TokenType::BOOL_TYPE},
    {"string", TokenType::STRING_TYPE}
};

Lexer::Lexer(const std::string& sourceCode)
    : source(sourceCode),
      current(0),
      line(1),
      column(1)
{
}

char Lexer::peek()
{
    if (current >= source.length())
        return '\0';

    return source[current];
}

char Lexer::peekNext()
{
    if (current + 1 >= source.length())
        return '\0';

    return source[current + 1];
}

char Lexer::advance()
{
    char c = peek();

    if (c == '\0')
        return '\0';

    current++;

    if (c == '\n')
    {
        line++;
        column = 1;
    }
    else
    {
        column++;
    }

    return c;
}

void Lexer::skipWhitespaceAndComments()
{
    while (true)
    {
        while (isspace(peek()))
            advance();

        // Single-line comment
        if (peek() == '/' && peekNext() == '/')
        {
            while (peek() != '\n' && peek() != '\0')
                advance();

            continue;
        }

        // Multi-line comment
        if (peek() == '/' && peekNext() == '*')
        {
            advance();
            advance();

            while (!(peek() == '*' && peekNext() == '/') &&
                   peek() != '\0')
            {
                advance();
            }

            if (peek() == '*' && peekNext() == '/')
            {
                advance();
                advance();
            }

            continue;
        }

        break;
    }
}

Token Lexer::scanIdentifierOrKeyword()
{
    int startLine = line;
    int startColumn = column;

    std::string text;

    while (isalnum(peek()) || peek() == '_')
        text += advance();

    auto it = keywords.find(text);

    if (it != keywords.end())
    {
        return Token(
            it->second,
            text,
            startLine,
            startColumn
        );
    }

    return Token(
        TokenType::IDENTIFIER,
        text,
        startLine,
        startColumn
    );
}

Token Lexer::scanNumber()
{
    int startLine = line;
    int startColumn = column;

    std::string number;
    bool isFloat = false;

    while (isdigit(peek()))
        number += advance();

    if (peek() == '.' && isdigit(peekNext()))
    {
        isFloat = true;
        number += advance();

        while (isdigit(peek()))
            number += advance();
    }

    return Token(
        isFloat
            ? TokenType::FLOAT_LITERAL
            : TokenType::INTEGER_LITERAL,
        number,
        startLine,
        startColumn
    );
}

Token Lexer::scanString()
{
    int startLine = line;
    int startColumn = column;

    advance(); // opening "

    std::string value;

    while (peek() != '"' && peek() != '\0')
        value += advance();

    if (peek() == '"')
    {
        advance();

        return Token(
            TokenType::STRING_LITERAL,
            value,
            startLine,
            startColumn
        );
    }

    return Token(
        TokenType::INVALID,
        value,
        startLine,
        startColumn
    );
}

Token Lexer::scanChar()
{
    int startLine = line;
    int startColumn = column;

    advance(); // opening '

    std::string value;

    if (peek() != '\'' && peek() != '\0')
        value += advance();

    if (peek() == '\'')
    {
        advance();

        return Token(
            TokenType::CHAR_LITERAL,
            value,
            startLine,
            startColumn
        );
    }

    return Token(
        TokenType::INVALID,
        value,
        startLine,
        startColumn
    );
}

Token Lexer::scanOperatorOrDelimiter()
{
    int startLine = line;
    int startColumn = column;

    char c = advance();

    switch (c)
    {
        case '+':
            if (peek() == '+')
            {
                advance();
                return Token(TokenType::INCREMENT, "++",
                             startLine, startColumn);
            }
            return Token(TokenType::PLUS, "+",
                         startLine, startColumn);

        case '-':
            if (peek() == '-')
            {
                advance();
                return Token(TokenType::DECREMENT, "--",
                             startLine, startColumn);
            }
            return Token(TokenType::MINUS, "-",
                         startLine, startColumn);

        case '*':
            return Token(TokenType::MULTIPLY, "*",
                         startLine, startColumn);

        case '/':
            return Token(TokenType::DIVIDE, "/",
                         startLine, startColumn);

        case '%':
            return Token(TokenType::MODULO, "%",
                         startLine, startColumn);

        case '=':
            if (peek() == '=')
            {
                advance();
                return Token(TokenType::EQUAL, "==",
                             startLine, startColumn);
            }
            return Token(TokenType::ASSIGN, "=",
                         startLine, startColumn);

        case '!':
            if (peek() == '=')
            {
                advance();
                return Token(TokenType::NOT_EQUAL, "!=",
                             startLine, startColumn);
            }
            return Token(TokenType::NOT, "!",
                         startLine, startColumn);

        case '<':
            if (peek() == '=')
            {
                advance();
                return Token(TokenType::LESS_EQUAL, "<=",
                             startLine, startColumn);
            }
            return Token(TokenType::LESS, "<",
                         startLine, startColumn);

        case '>':
            if (peek() == '=')
            {
                advance();
                return Token(TokenType::GREATER_EQUAL, ">=",
                             startLine, startColumn);
            }
            return Token(TokenType::GREATER, ">",
                         startLine, startColumn);

        case '&':
            if (peek() == '&')
            {
                advance();
                return Token(TokenType::AND, "&&",
                             startLine, startColumn);
            }
            break;

        case '|':
            if (peek() == '|')
            {
                advance();
                return Token(TokenType::OR, "||",
                             startLine, startColumn);
            }
            break;

        case '(':
            return Token(TokenType::LEFT_PAREN, "(",
                         startLine, startColumn);

        case ')':
            return Token(TokenType::RIGHT_PAREN, ")",
                         startLine, startColumn);

        case '{':
            return Token(TokenType::LEFT_BRACE, "{",
                         startLine, startColumn);

        case '}':
            return Token(TokenType::RIGHT_BRACE, "}",
                         startLine, startColumn);

        case ';':
            return Token(TokenType::SEMICOLON, ";",
                         startLine, startColumn);

        case ',':
            return Token(TokenType::COMMA, ",",
                         startLine, startColumn);
    }

    return Token(
        TokenType::INVALID,
        std::string(1, c),
        startLine,
        startColumn
    );
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (peek() != '\0')
    {
        skipWhitespaceAndComments();

        if (peek() == '\0')
            break;

        int startLine = line;
        int startColumn = column;

        char c = peek();

        if (isalpha(c) || c == '_')
        {
            tokens.push_back(scanIdentifierOrKeyword());
        }
        else if (isdigit(c))
        {
            tokens.push_back(scanNumber());
        }
        else if (c == '"')
        {
            tokens.push_back(scanString());
        }
        else if (c == '\'')
        {
            tokens.push_back(scanChar());
        }
        else
        {
            tokens.push_back(scanOperatorOrDelimiter());
        }
    }

    tokens.push_back(
        Token(
            TokenType::END_OF_FILE,
            "",
            line,
            column
        )
    );

    return tokens;
}

std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
        case TokenType::BREW: return "BREW";
        case TokenType::SERVE: return "SERVE";
        case TokenType::SIP: return "SIP";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::WHILE: return "WHILE";
        case TokenType::RETURN: return "RETURN";
        case TokenType::BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";

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