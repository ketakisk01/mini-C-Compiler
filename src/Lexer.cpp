#include "Lexer.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {
    keywords = {
        "int", "float", "char", "bool",
        "if", "else", "while", "print", "return"
    };
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return source[position];
}

char Lexer::peekNext() const {
    if (position + 1 >= source.length()) return '\0';
    return source[position + 1];
}

char Lexer::advance() {
    if (isAtEnd()) return '\0';
    char current = source[position++];
    if (current == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return current;
}

bool Lexer::isAtEnd() const {
    return position >= source.length();
}

void Lexer::skipWhitespaceAndComments() {
    // Only skip standard whitespace here. Comments are handled in the main tokenization loop
    // to properly capture lexical errors like unclosed multi-line comments.
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else {
            break;
        }
    }
}

Token Lexer::scanNumber() {
    int startLine = line;
    int startCol = column - 1; // First digit was already advanced
    std::string lexeme = "";
    lexeme += source[position - 1];

    bool isFloat = false;

    while (!isAtEnd() && std::isdigit(peek())) {
        lexeme += advance();
    }

    if (peek() == '.') {
        isFloat = true;
        lexeme += advance(); // consume '.'
        if (isAtEnd() || !std::isdigit(peek())) {
            return Token(TokenType::INVALID, "Malformed float literal: " + lexeme, startLine, startCol);
        }
        while (!isAtEnd() && std::isdigit(peek())) {
            lexeme += advance();
        }
    }

    if (isFloat) {
        return Token(TokenType::FLOAT, lexeme, startLine, startCol);
    } else {
        return Token(TokenType::INTEGER, lexeme, startLine, startCol);
    }
}

Token Lexer::scanIdentifierOrKeyword() {
    int startLine = line;
    int startCol = column - 1; // First char was already advanced
    std::string lexeme = "";
    lexeme += source[position - 1];

    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        lexeme += advance();
    }

    if (keywords.count(lexeme) > 0) {
        return Token(TokenType::KEYWORD, lexeme, startLine, startCol);
    } else if (lexeme == "true" || lexeme == "false") {
        return Token(TokenType::BOOLEAN, lexeme, startLine, startCol);
    }
    return Token(TokenType::IDENTIFIER, lexeme, startLine, startCol);
}

Token Lexer::scanString() {
    int startLine = line;
    int startCol = column - 1; // '"' was already advanced
    std::string lexeme = "\"";
    bool closed = false;

    while (!isAtEnd()) {
        char c = peek();
        if (c == '"') {
            lexeme += advance(); // consume '"'
            closed = true;
            break;
        }
        if (c == '\n') {
            // Strings cannot span across lines
            break;
        }
        lexeme += advance();
    }

    if (!closed) {
        return Token(TokenType::INVALID, "Unclosed string literal: " + lexeme, startLine, startCol);
    }
    return Token(TokenType::STRING, lexeme, startLine, startCol);
}

Token Lexer::scanChar() {
    int startLine = line;
    int startCol = column - 1; // '\'' was already advanced
    std::string lexeme = "'";
    bool closed = false;

    if (!isAtEnd() && peek() != '\'') {
        if (peek() == '\\') {
            lexeme += advance(); // consume '\\'
            if (!isAtEnd()) {
                lexeme += advance(); // consume escaped char
            }
        } else {
            lexeme += advance();
        }
    }

    if (!isAtEnd() && peek() == '\'') {
        lexeme += advance(); // consume '\''
        closed = true;
    }

    if (!closed || lexeme.length() < 3) {
        return Token(TokenType::INVALID, "Invalid character literal: " + lexeme, startLine, startCol);
    }
    return Token(TokenType::CHAR, lexeme, startLine, startCol);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;

        int tokenLine = line;
        int tokenCol = column;
        char c = advance();

        // 1. Identifiers and Keywords
        if (std::isalpha(c) || c == '_') {
            tokens.push_back(scanIdentifierOrKeyword());
        }
        // 2. Numbers
        else if (std::isdigit(c)) {
            tokens.push_back(scanNumber());
        }
        // 3. String literals
        else if (c == '"') {
            tokens.push_back(scanString());
        }
        // 4. Character literals
        else if (c == '\'') {
            tokens.push_back(scanChar());
        }
        // 5. Delimiters
        else if (c == '(' || c == ')' || c == '{' || c == '}' || c == ';' || c == ',') {
            std::string lexeme(1, c);
            tokens.push_back(Token(TokenType::DELIMITER, lexeme, tokenLine, tokenCol));
        }
        // 6. Comments & Operators
        else if (c == '/') {
            if (peek() == '/') {
                // Single-line comment: consume '/' and advance to end of line
                advance();
                while (!isAtEnd() && peek() != '\n') {
                    advance();
                }
            } else if (peek() == '*') {
                // Multi-line comment: consume '*' and find matching '*/'
                advance();
                bool closed = false;
                while (!isAtEnd()) {
                    if (peek() == '*' && peekNext() == '/') {
                        advance(); // consume '*'
                        advance(); // consume '/'
                        closed = true;
                        break;
                    }
                    advance();
                }
                if (!closed) {
                    tokens.push_back(Token(TokenType::INVALID, "Unclosed multi-line comment", tokenLine, tokenCol));
                }
            } else {
                // Division Operator
                tokens.push_back(Token(TokenType::OPERATOR, "/", tokenLine, tokenCol));
            }
        }
        // Operators
        else if (c == '+') {
            if (peek() == '+') {
                advance();
                tokens.push_back(Token(TokenType::OPERATOR, "++", tokenLine, tokenCol));
            } else {
                tokens.push_back(Token(TokenType::OPERATOR, "+", tokenLine, tokenCol));
            }
        }
        else if (c == '-') {
            if (peek() == '-') {
                advance();
                tokens.push_back(Token(TokenType::OPERATOR, "--", tokenLine, tokenCol));
            } else {
                tokens.push_back(Token(TokenType::OPERATOR, "-", tokenLine, tokenCol));
            }
        }
        else if (c == '*') {
            tokens.push_back(Token(TokenType::OPERATOR, "*", tokenLine, tokenCol));
        }
        else if (c == '%') {
            tokens.push_back(Token(TokenType::OPERATOR, "%", tokenLine, tokenCol));
        }
        else if (c == '=') {
            if (peek() == '=') {
                advance();
                tokens.push_back(Token(TokenType::OPERATOR, "==", tokenLine, tokenCol));
            } else {
                tokens.push_back(Token(TokenType::OPERATOR, "=", tokenLine, tokenCol));
            }
        }
        else if (c == '!') {
            if (peek() == '=') {
                advance();
                tokens.push_back(Token(TokenType::OPERATOR, "!=", tokenLine, tokenCol));
            } else {
                tokens.push_back(Token(TokenType::OPERATOR, "!", tokenLine, tokenCol));
            }
        }
        else if (c == '<') {
            if (peek() == '=') {
                advance();
                tokens.push_back(Token(TokenType::OPERATOR, "<=", tokenLine, tokenCol));
            } else {
                tokens.push_back(Token(TokenType::OPERATOR, "<", tokenLine, tokenCol));
            }
        }
        else if (c == '>') {
            if (peek() == '=') {
                advance();
                tokens.push_back(Token(TokenType::OPERATOR, ">=", tokenLine, tokenCol));
            } else {
                tokens.push_back(Token(TokenType::OPERATOR, ">", tokenLine, tokenCol));
            }
        }
        else if (c == '&') {
            if (peek() == '&') {
                advance();
                tokens.push_back(Token(TokenType::OPERATOR, "&&", tokenLine, tokenCol));
            } else {
                tokens.push_back(Token(TokenType::INVALID, "Invalid character '&', did you mean '&&'?", tokenLine, tokenCol));
            }
        }
        else if (c == '|') {
            if (peek() == '|') {
                advance();
                tokens.push_back(Token(TokenType::OPERATOR, "||", tokenLine, tokenCol));
            } else {
                tokens.push_back(Token(TokenType::INVALID, "Invalid character '|', did you mean '||'?", tokenLine, tokenCol));
            }
        }
        // Invalid single characters
        else {
            std::string lexeme(1, c);
            tokens.push_back(Token(TokenType::INVALID, "Unexpected character: '" + lexeme + "'", tokenLine, tokenCol));
        }
    }

    tokens.push_back(Token(TokenType::EOF_TOKEN, "EOF", line, column));
    return tokens;
}
