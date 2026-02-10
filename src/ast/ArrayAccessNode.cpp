#include "ast/ArrayAccessNode.hpp"
#include "ir/Tac.hpp"

Operand ArrayAccessNode::generateIR(CFG &graph, SymbolTable &st) {
    auto arrayName = array->generateIR(graph, st);
    auto indexName = index->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    graph.addInstruction(new ArrayAccessTac(name, arrayName, indexName));
    return name;
}
