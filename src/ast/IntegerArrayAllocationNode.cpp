#include "ast/IntegerArrayAllocationNode.hpp"
#include "ir/Tac.hpp"

Operand IntegerArrayAllocationNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addVariable("int[]", name);
    auto lengthName = length->generateIR(graph, st);
    graph.addInstruction(new NewArrayTac(name, lengthName));
    return name;
}
