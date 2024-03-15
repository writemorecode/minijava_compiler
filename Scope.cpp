#include "Scope.hpp"

#include <iostream>

#include "Class.hpp"
#include "Method.hpp"
#include "Variable.hpp"

Scope::Scope(Record *record, Scope *parent, std::string name)
    : record{record}, parent{parent}, scopeName{std::move(name)} {};

Scope *Scope::getParent() { return parent; }

Scope *Scope::nextChild(const std::string &name, Record *record) {
    Scope *nextChild = nullptr;
    if (next == children.size()) {
        children.push_back(std::make_unique<Scope>(record, this, name));
        nextChild = children.back().get();
    } else {
        nextChild = children[next].get();
    }
    next++;
    return nextChild;
}

void Scope::addVariable(const std::string &type, const std::string &id) {
    variables.emplace(id, Variable(type, id));
}
void Scope::addMethod(const std::string &type, const std::string &id) {
    methods.emplace(id, Method(type, id));
}
void Scope::addClass(const std::string &id) { classes.emplace(id, Class(id)); }

Variable *Scope::lookupVariable(const std::string &key) {
    auto it = variables.find(key);
    if (it != variables.end()) {
        return &(it->second);
    }
    if (parent == nullptr) {
        return nullptr;
    }
    return parent->lookupVariable(key);
}

Method *Scope::lookupMethod(const std::string &key) {
    auto it = methods.find(key);
    if (it != methods.end()) {
        return &(it->second);
    }
    if (parent == nullptr) {
        return nullptr;
    }
    return parent->lookupMethod(key);
}

Class *Scope::lookupClass(const std::string &key) {
    auto it = classes.find(key);
    if (it != classes.end()) {
        return &(it->second);
    }
    if (parent == nullptr) {
        return nullptr;
    }
    return parent->lookupClass(key);
}

Variable *Scope::lookupVariableInScope(const std::string &key) {
    auto it = variables.find(key);
    if (it != variables.end()) {
        return &(it->second);
    }
    return nullptr;
}

void Scope::resetScope() {
    next = 0;
    for (auto &child : children) {
        child->resetScope();
    }
}

std::string Scope::getName() const { return scopeName; }

Record *Scope::getRecord() const { return record; }

void Scope::printScope(int &count, std::ostream &os) const {
    int id = count;
    os << "n" << id << "[label=\"Symbol table: (" << scopeName << ")\\n";

    os << "ID\tType\tRecord\n";
    for (const auto &var : variables) {
        (var.second).printRecord(os);
        os << "\\n";
    }
    for (const auto &method : methods) {
        (method.second).printRecord(os);
        os << "\\n";
    }
    for (const auto &class_ : classes) {
        (class_.second).printRecord(os);
        os << "\\n";
    }
    os << "\"];\n";
    for (const auto &child : children) {
        int n = ++count;
        child->printScope(count, os);
        os << "n" << id << " -> n" << n << "\n";
    }
}
