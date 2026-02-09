#include "ast/ArithmeticExpressionNode.hpp"

#include "ir/ArithmeticTac.hpp"

Operand PlusNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    graph.addInstruction(new AddTac(name, lhs_name, rhs_name));
    return name;
}
Operand MinusNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    graph.addInstruction(new SubtractTac(name, lhs_name, rhs_name));
    return name;
}
Operand MultiplicationNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    graph.addInstruction(new MultiplyTac(name, lhs_name, rhs_name));
    return name;
}
Operand DivisionNode::generateIR(CFG &graph, SymbolTable &st) {
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    graph.addInstruction(new DivideTac(name, lhs_name, rhs_name));
    return name;
}
