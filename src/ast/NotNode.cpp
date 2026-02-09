#include "ast/NotNode.hpp"
#include "ir/Tac.hpp"

Operand NotNode::generateIR(CFG &graph, SymbolTable &st) {
    auto rhsName = expr->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    graph.addInstruction(new NotTac(name, rhsName));
    return name;
}
