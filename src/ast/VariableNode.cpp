#include "ast/VariableNode.hpp"

#include "ast/AstVisitor.hpp"

void VariableNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }

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
