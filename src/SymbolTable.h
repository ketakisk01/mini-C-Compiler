#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>

struct Symbol {
    std::string name;
    std::string type; // "int", "float", "char", "bool"
    int line;
    int column;
};

class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    std::vector<Symbol> masterList; // Holds all variables declared during the lifetime of parsing for printout

public:
    SymbolTable();

    void enterScope();
    void exitScope();

    // Inserts a new symbol in the current scope. Returns false if duplicate in current scope.
    bool insert(const std::string& name, const std::string& type, int line, int column);

    // Looks up a symbol in all visible scopes (from inner to outer). Returns true if found.
    bool lookup(const std::string& name, Symbol& outSymbol) const;

    // Checks if the symbol exists only in the current active scope.
    bool existsInCurrentScope(const std::string& name) const;

    // Flattens all symbols currently stored across all scopes for visualization.
    std::vector<Symbol> getAllSymbols() const;
};

#endif // SYMBOL_TABLE_H
