#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <string>
#include <vector>

class Lexer {
private:
    std::string source;

    size_t position;
    int line;
    int column;

    char currentChar();

    void advance();

    void skipWhitespace();

    void skipComment();

    Token scanIdentifierOrKeyword();

    Token scanNumber();

    Token scanString();

    Token scanCharacter();

    Token scanOperatorOrDelimiter();

public:
    Lexer(const std::string& sourceCode);

    std::vector<Token> tokenize();
};

#endif