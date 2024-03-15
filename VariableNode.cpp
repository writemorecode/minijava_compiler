#include "VariableNode.hpp"

bool VariableNode::buildTable(SymbolTable &st) const {

    Variable *lookup = st.lookupVariableInScope(name->value);
    if (lookup) {
        std::cerr << "Error: (line " << lineno << ") "
                  << "Variable '" << name->value << "' "
                  << "already declared.\n";
        return false;
    }
    st.addVariable(type->value, name->value);
    Variable *currentVariable = st.lookupVariable(name->value);

    Record *curRecord = st.getCurrentRecord();
    if (curRecord->getType() == curRecord->getID()) {
        // Record is a class
        auto *curClass = dynamic_cast<Class *>(curRecord);
        curClass->addVariable(currentVariable);
    } else {
        // Record is a method
        auto *curMethod = dynamic_cast<Method *>(curRecord);
        curMethod->addVariable(currentVariable);
    }

    return true;
};

std::string VariableNode::checkTypes(SymbolTable &st) const {
    const auto &variableType = type->value;
    const bool hasBaseType =
        (variableType == "int" || variableType == "int[]" ||
         variableType == "boolean");
    auto *classLookup = st.lookupClass(variableType);
    if (!hasBaseType && !classLookup) {
        std::cout << "Error: (line " << lineno << ") "
                  << "Unknown type '" << variableType << "' "
                  << "for identifier '" << name->value << "'.\n";
        return "";
    }
    return variableType;
}
