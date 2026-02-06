#include <iostream>

#include "semantic/Scope.hpp"

#include "semantic/Class.hpp"
#include "semantic/Method.hpp"
#include "semantic/Variable.hpp"

Scope *Scope::getParent() { return parent; }

Scope *Scope::nextChild(const std::string &name, Record *record) {
    if (const auto &it = children.find(name); it != children.end()) {
        return it->second.get();
    }
    const auto &result =
        children.insert({name, std::make_unique<Scope>(name, record, this)});
    const auto &it = result.first;
    const auto &uptr = it->second;
    return uptr.get();
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
    for (const auto &[_, child] : children) {
        int n = ++count;
        child->printScope(count, os);
        os << "n" << id << " -> n" << n << "\n";
    }
}

std::set<std::string> Scope::getVariableNames() const {
    std::set<std::string> variableNames;
    for (const auto &variable : variables) {
        variableNames.insert(variable.first);
    }
    return variableNames;
}

std::set<std::string> Scope::getMethodNames() const {
    std::set<std::string> methodNames;
    for (const auto &method : methods) {
        methodNames.insert(method.first);
    }
    return methodNames;
}

std::set<std::string> Scope::getClassNames() const {
    std::set<std::string> classNames;
    for (const auto &class_ : classes) {
        classNames.insert(class_.first);
    }
    return classNames;
}

std::vector<const Scope *> Scope::getChildren() const {
    std::vector<const Scope *> childScopes;
    childScopes.reserve(children.size());
    for (const auto &entry : children) {
        childScopes.push_back(entry.second.get());
    }
    return childScopes;
}
