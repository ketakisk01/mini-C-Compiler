#include "SymbolTable.h"
#include <iostream>

SymbolTable::SymbolTable()
{
    // Initialize with global scope (level 0)
    scopeStack.push_back({});
}

void SymbolTable::enterScope()
{
    scopeStack.push_back({});
}

void SymbolTable::exitScope()
{
    if (scopeStack.size() > 1)
    {
        scopeStack.pop_back();
    }
}

int SymbolTable::getCurrentScopeLevel() const
{
    return static_cast<int>(scopeStack.size()) - 1;
}

void SymbolTable::addSymbol(
    const std::string& name,
    const std::string& kind,
    int line
)
{
    addSymbol(name, kind, line, "unknown", getCurrentScopeLevel());
}

bool SymbolTable::addSymbol(
    const std::string& name,
    const std::string& kind,
    int line,
    const std::string& type,
    int scopeLevel
)
{
    Symbol sym(name, kind, line, type, scopeLevel);

    if (scopeStack.empty())
    {
        scopeStack.push_back({});
    }

    // Insert or update in current scope
    scopeStack.back()[name] = sym;

    // Record in sequential list for table printing
    bool existsInSymbols = false;
    for (auto& s : symbols)
    {
        if (s.name == name && s.scopeLevel == scopeLevel)
        {
            s = sym;
            existsInSymbols = true;
            break;
        }
    }

    if (!existsInSymbols)
    {
        symbols.push_back(sym);
    }

    return true;
}

bool SymbolTable::contains(
    const std::string& name
) const
{
    return getSymbol(name) != nullptr;
}

bool SymbolTable::containsInCurrentScope(
    const std::string& name
) const
{
    if (scopeStack.empty())
        return false;

    return scopeStack.back().find(name) != scopeStack.back().end();
}

const Symbol* SymbolTable::getSymbol(
    const std::string& name
) const
{
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it)
    {
        auto found = it->find(name);
        if (found != it->end())
        {
            return &(found->second);
        }
    }

    return nullptr;
}

Symbol* SymbolTable::getSymbol(
    const std::string& name
)
{
    for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it)
    {
        auto found = it->find(name);
        if (found != it->end())
        {
            return &(found->second);
        }
    }

    return nullptr;
}

const std::vector<Symbol>& SymbolTable::getSymbols() const
{
    return symbols;
}

void SymbolTable::clear()
{
    symbols.clear();
    scopeStack.clear();
    scopeStack.push_back({});
}

void SymbolTable::print() const
{
    std::cout << "\n========== SYMBOL TABLE ==========\n";
    std::cout << "Name\tKind\tLine\n";

    for (const auto& symbol : symbols)
    {
        std::cout
            << symbol.name << "\t"
            << symbol.kind << "\t"
            << symbol.line << "\n";
    }

    std::cout << "==================================\n";
}