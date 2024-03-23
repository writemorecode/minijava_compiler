#include "ArithmeticExpressionNode.hpp"

#include "ArithmeticTac.hpp"

std::string ArithmeticExpressionNode::checkTypes(SymbolTable &st) const {
    const auto lhsType = left->checkTypes(st);
    const auto rhsType = right->checkTypes(st);

    if (!lhsType.empty() && !rhsType.empty()) {
        if (lhsType == "int" && rhsType == "int") {
            return "int";
        }
    }
    std::cerr << "Error: ";
    std::cerr << "(line " << lineno << ") ";
    std::cerr << type << " operation does not support operands of types ";
    std::cerr << "'" << lhsType << "' and '" << rhsType << "'.\n";
    return "";
}

Operand PlusNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    graph.addInstruction(new AddTac(name, lhs_name, rhs_name));
    return name;
}
Operand MinusNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    graph.addInstruction(new SubtractTac(name, lhs_name, rhs_name));
    return name;
}
Operand MultiplicationNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    graph.addInstruction(new MultiplyTac(name, lhs_name, rhs_name));
    return name;
}
Operand DivisionNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    graph.addInstruction(new DivideTac(name, lhs_name, rhs_name));
    return name;
}
