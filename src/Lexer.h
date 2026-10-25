#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class Lexer
{
private:
    std::string source;

    size_t current;
    int line;
    int column;

    // Set when a /* ... */ comment is never closed
    int unterminatedCommentLine = 0;
    int unterminatedCommentColumn = 0;

    char peek();
    char peekNext();
    char advance();

    void skipWhitespaceAndComments();

    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanString();
    Token scanChar();
    Token scanOperatorOrDelimiter();

public:
    Lexer(const std::string& sourceCode);

    std::vector<Token> tokenize();
};

#endif