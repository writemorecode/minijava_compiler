#include "ast/LogicalExpressionNode.hpp"
#include "ir/LogicalTac.hpp"

Operand LessThanNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    graph.addInstruction(new LessThanTac(name, lhs_name, rhs_name));
    return name;
}
Operand GreaterThanNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    graph.addInstruction(new GreaterThanTac(name, lhs_name, rhs_name));
    return name;
}
Operand EqualToNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    graph.addInstruction(new EqualToTac(name, lhs_name, rhs_name));
    return name;
}
