#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <unordered_map>

struct Symbol
{
    std::string name;
    std::string kind;
    int line;
    std::string type;
    int scopeLevel;

    Symbol(
        const std::string& name = "",
        const std::string& kind = "variable",
        int line = 0,
        const std::string& type = "unknown",
        int scopeLevel = 0
    )
        : name(name),
          kind(kind),
          line(line),
          type(type),
          scopeLevel(scopeLevel)
    {
    }
};

class SymbolTable
{
private:
    std::vector<Symbol> symbols;
    std::vector<std::unordered_map<std::string, Symbol>> scopeStack;

public:
    SymbolTable();

    // Scope management
    void enterScope();
    void exitScope();
    int getCurrentScopeLevel() const;

    // Symbol addition
    void addSymbol(
        const std::string& name,
        const std::string& kind,
        int line
    );

    bool addSymbol(
        const std::string& name,
        const std::string& kind,
        int line,
        const std::string& type,
        int scopeLevel
    );

    // Lookups
    bool contains(
        const std::string& name
    ) const;

    bool containsInCurrentScope(
        const std::string& name
    ) const;

    const Symbol* getSymbol(
        const std::string& name
    ) const;

    Symbol* getSymbol(
        const std::string& name
    );

    const std::vector<Symbol>& getSymbols() const;

    void clear();

    void print() const;
};

#endif