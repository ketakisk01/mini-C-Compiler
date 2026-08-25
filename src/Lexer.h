#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "Token.h"

class Lexer {
private:
    std::string source;
    size_t position;
    int line;
    int column;

    std::unordered_set<std::string> keywords;

    char peek() const;
    char peekNext() const;
    char advance();
    bool isAtEnd() const;
    void skipWhitespaceAndComments();
    Token scanNumber();
    Token scanIdentifierOrKeyword();
    Token scanString();
    Token scanChar();

public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();
};

#endif // LEXER_H
