#include "SymbolTable.hpp"

#include <iostream>

#include "Scope.hpp"

SymbolTable::SymbolTable()
    : root{std::make_unique<Scope>()}, current{root.get()} {}

void SymbolTable::enterScope(const std::string &name, Record *record) {
    current = current->nextChild(name, record);
}

void SymbolTable::enterClassScope(Class *scopeClass) {
    auto name = "Class: " + scopeClass->getID();
    enterScope(name, scopeClass);
}
void SymbolTable::enterClassScope(const std::string &scopeName,
                                  Record *record) {
    auto name = "Class: " + scopeName;
    enterScope(name, record);
}

void SymbolTable::enterMethodScope(Method *scopeMethod) {
    auto name = "Method: " + scopeMethod->getID();
    enterScope(name, scopeMethod);
}
void SymbolTable::enterMethodScope(const std::string &scopeName,
                                   Record *record) {
    auto name = "Method: " + scopeName;
    enterScope(name, record);
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

Scope *SymbolTable::resolveScope(const std::string &className,
                                 const std::string &methodName) {
    auto *classLookup = lookupClass(className);
    if (classLookup == nullptr) {
        return nullptr;
    }

    enterClassScope(classLookup);

    auto *methodLookup = classLookup->lookupMethod(methodName);
    if (methodLookup == nullptr) {
        return nullptr;
    }
    enterMethodScope(methodLookup);

    Scope *scope = getCurrentScope();
    exitScope();
    exitScope();

    return scope;
}
