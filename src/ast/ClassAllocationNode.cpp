#include "ast/ClassAllocationNode.hpp"
#include "ir/Tac.hpp"

std::string ClassAllocationNode::checkTypes(SymbolTable &st) const {
    auto *classLookup = st.lookupClass(id);
    if (classLookup == nullptr) {
        std::cerr << "Error: (line " << lineno << ") Unknown class '" << id
                  << "'.\n";
        return "";
    }
    return id;
}

Operand ClassAllocationNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addVariable(id, name);
    graph.addInstruction(new NewTac(name, id));
    return name;
}
