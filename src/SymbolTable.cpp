#include "SymbolTable.h"

SymbolTable::SymbolTable() {
    // Initialize the global scope
    enterScope();
}

void SymbolTable::enterScope() {
    scopes.push_back(std::unordered_map<std::string, Symbol>());
}

void SymbolTable::exitScope() {
    if (scopes.size() > 1) {
        scopes.pop_back();
    }
}

bool SymbolTable::insert(const std::string& name, const std::string& type, int line, int column) {
    if (existsInCurrentScope(name)) {
        return false;
    }
    Symbol sym{name, type, line, column};
    scopes.back()[name] = sym;
    masterList.push_back(sym);
    return true;
}

bool SymbolTable::lookup(const std::string& name, Symbol& outSymbol) const {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto search = it->find(name);
        if (search != it->end()) {
            outSymbol = search->second;
            return true;
        }
    }
    return false;
}

bool SymbolTable::existsInCurrentScope(const std::string& name) const {
    if (scopes.empty()) return false;
    return scopes.back().count(name) > 0;
}

std::vector<Symbol> SymbolTable::getAllSymbols() const {
    return masterList;
}
