#include "Class.hpp"

void Class::addVariable(Variable *variable) {
    const auto id = variable->getID();
    variables.insert({id, variable});
}

void Class::addMethod(Method *method) {
    const auto id = method->getID();
    methods.insert({id, method});
}

Variable *Class::lookupVariable(const std::string &id) {
    auto it = variables.find(id);
    if (it == variables.end()) {
        return nullptr;
    }
    return it->second;
}

Method *Class::lookupMethod(const std::string &id) {
    auto it = methods.find(id);
    if (it == methods.end()) {
        return nullptr;
    }
    return it->second;
}

std::string Class::getRecord() const { return "Class"; }
