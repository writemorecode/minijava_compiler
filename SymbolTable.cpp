#include "SymbolTable.hpp"

#include <iostream>

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

void SymbolTable::addIntegerVariable(const std::string &id) {
    current->addVariable("int", id);
}

void SymbolTable::addBooleanVariable(const std::string &id) {
    current->addVariable("boolean", id);
}

void SymbolTable::printTable(std::ostream &os) const {
    int count = 0;
    auto *scope = root.get();
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

Variable *SymbolTable::lookupVariable(const std::string &id) {
    return current->lookupVariable(id);
}
Method *SymbolTable::lookupMethod(const std::string &id) {
    return current->lookupMethod(id);
}
Class *SymbolTable::lookupClass(const std::string &id) {
    return current->lookupClass(id);
}
Variable *SymbolTable::lookupVariableInScope(const std::string &key) {
    return current->lookupVariableInScope(key);
}

Record *SymbolTable::getCurrentRecord() const { return current->getRecord(); }

const Method *SymbolTable::getMethodFromQualifiedName(const std::string &name) {
    auto splitStringOnPeriod = [](const std::string &str) {
        const auto index = str.find('.');
        const auto &first = str.substr(0, index);
        const auto &second = str.substr(index + 1);
        return std::make_pair(first, second);
    };

    const auto &[className, methodName] = splitStringOnPeriod(name);

    auto *classLookup = lookupClass(className);
    if (classLookup == nullptr) {
        return nullptr;
    }

    enterScope("Class: " + className, classLookup);
    const auto *methodLookup = lookupMethod(methodName);
    exitScope();
    if (methodLookup == nullptr) {
        return nullptr;
    }

    return methodLookup;
}
