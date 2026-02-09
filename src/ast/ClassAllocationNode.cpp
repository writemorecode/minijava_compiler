#include "ast/ClassAllocationNode.hpp"
#include "ir/Tac.hpp"

Operand ClassAllocationNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addVariable(id, name);
    graph.addInstruction(new NewTac(name, id));
    return name;
}
