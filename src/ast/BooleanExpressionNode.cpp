#include "ast/BooleanExpressionNode.hpp"
#include "ir/BooleanTac.hpp"

Operand AndNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    graph.addInstruction(new AndTac(name, lhs_name, rhs_name));
    return name;
}

Operand OrNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    graph.addInstruction(new OrTac(name, lhs_name, rhs_name));
    return name;
}
