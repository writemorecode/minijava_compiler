#include "ast/ArrayLengthNode.hpp"
#include "ir/Tac.hpp"

Operand ArrayLengthNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    auto arrayName = array->value;
    graph.addInstruction(new ArrayLengthTac(name, arrayName));
    return name;
}
