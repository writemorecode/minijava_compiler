#include "ArithmeticExpressionNode.hpp"

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

std::string PlusNode::generateIR(CFG &graph) {
    auto name = graph.getTemporaryName();
    auto lhs_name = left->generateIR(graph);
    auto rhs_name = right->generateIR(graph);
    graph.addInstruction(new ExpressionTac(name, lhs_name, "+", rhs_name));
    return name;
}
std::string MinusNode::generateIR(CFG &graph) {
    auto name = graph.getTemporaryName();
    auto lhs_name = left->generateIR(graph);
    auto rhs_name = right->generateIR(graph);
    graph.addInstruction(new ExpressionTac(name, lhs_name, "-", rhs_name));
    return name;
}
std::string MultiplicationNode::generateIR(CFG &graph) {
    auto name = graph.getTemporaryName();
    auto lhs_name = left->generateIR(graph);
    auto rhs_name = right->generateIR(graph);
    graph.addInstruction(new ExpressionTac(name, lhs_name, "*", rhs_name));
    return name;
}
std::string DivisionNode::generateIR(CFG &graph) {
    auto name = graph.getTemporaryName();
    auto lhs_name = left->generateIR(graph);
    auto rhs_name = right->generateIR(graph);
    graph.addInstruction(new ExpressionTac(name, lhs_name, "/", rhs_name));
    return name;
}
