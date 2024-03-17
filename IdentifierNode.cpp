#include "IdentifierNode.hpp"

#include <algorithm>
#include <utility>

std::string IdentifierNode::checkTypes(SymbolTable &st) const {
    auto variableLookup = st.lookupVariable(value);
    if (variableLookup) {
        return variableLookup->getType();
    }
    auto classLookup = st.lookupClass(value);
    if (classLookup) {
        return classLookup->getType();
    }
    auto methodLookup = st.lookupMethod(value);
    if (methodLookup) {
        return methodLookup->getType();
    }

    std::cerr << "Error: ";
    std::cerr << "(line " << lineno << ") ";
    std::cerr << "Undeclared identifier " << value << ".\n";
    return "";
}

std::string IdentifierNode::generateIR(CFG &graph, SymbolTable &st) { return value; }
