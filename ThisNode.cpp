#include "ThisNode.hpp"
#include "Tac.hpp"

std::string ThisNode::checkTypes(SymbolTable &st) const {
    auto const *lookup = st.lookupVariable(type);
    if (!lookup) {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Undeclared identifier (type " << type << ")" << value
                  << ".\n";
        return "";
    }
    return lookup->getType();
}

std::string ThisNode::generateIR(CFG &graph, SymbolTable &st) { return value; }
