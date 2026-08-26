#include "SymbolTable.h"
#include <iostream>

void SymbolTable::addSymbol(
    const std::string& name,
    const std::string& kind,
    int line
) {
    if (!contains(name)) {

        symbols.push_back({
            name,
            kind,
            line
        });
    }
}

bool SymbolTable::contains(const std::string& name) const {

    for (const Symbol& symbol : symbols) {

        if (symbol.name == name) {
            return true;
        }
    }

    return false;
}

void SymbolTable::print() const {

    std::cout << "\n===== SYMBOL TABLE =====\n";

    std::cout
        << "Name\tKind\tLine\n";

    std::cout
        << "---------------------------\n";

    for (const Symbol& symbol : symbols) {

        std::cout
            << symbol.name << "\t"
            << symbol.kind << "\t"
            << symbol.line << "\n";
    }
}