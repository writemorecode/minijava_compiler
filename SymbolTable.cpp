#include "SymbolTable.hpp"

#include <iostream>

#include "Node.h"
#include "Scope.hpp"

SymbolTable::SymbolTable()
    : root{std::make_unique<Scope>()}, current{root.get()} {}

void SymbolTable::enterScope(const std::string &name, Record *record) {
    current = current->nextChild(name, record);
}

void SymbolTable::exitScope() { current = current->getParent(); }

void SymbolTable::addVariable(const std::string &type, const std::string &id) {
    current->addVariable(type, id);
}
void SymbolTable::addMethod(const std::string &type, const std::string &id) {
    current->addMethod(type, id);
}
void SymbolTable::addClass(const std::string &id) { current->addClass(id); }

void SymbolTable::printTable(std::ostream &os) const {
    int count = 0;
    const auto scope = root.get();
    os << "digraph {\n";
    scope->printScope(count, os);
    os << "}\n";
}
void SymbolTable::resetTable() { current->resetScope(); }

std::string SymbolTable::getCurrentScopeName() const {
    return current->getName();
}
Scope *SymbolTable::getParentScope() const { return current->getParent(); }
Scope *SymbolTable::getCurrentScope() const { return current; }

Variable *SymbolTable::lookupVariable(const std::string &key) {
    return current->lookupVariable(key);
}
Method *SymbolTable::lookupMethod(const std::string &key) {
    return current->lookupMethod(key);
}
Class *SymbolTable::lookupClass(const std::string &key) {
    return current->lookupClass(key);
}
Variable *SymbolTable::lookupVariableInScope(const std::string &key) {
    return current->lookupVariableInScope(key);
}

Record *SymbolTable::getCurrentRecord() const { return current->getRecord(); }
