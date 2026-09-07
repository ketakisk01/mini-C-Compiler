#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>

struct Symbol
{
    std::string name;
    std::string kind;
    int line;
};

class SymbolTable
{
private:
    std::vector<Symbol> symbols;

public:
    void addSymbol(
        const std::string& name,
        const std::string& kind,
        int line
    );

    bool contains(
        const std::string& name
    ) const;

    void print() const;
};

#endif