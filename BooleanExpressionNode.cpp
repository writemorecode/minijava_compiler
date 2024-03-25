#include "BooleanExpressionNode.hpp"
#include "BooleanTac.hpp"

std::string BooleanExpressionNode::checkTypes(SymbolTable &st) const {
    const auto lhsType = left->checkTypes(st);
    const auto rhsType = right->checkTypes(st);

    if (lhsType == "boolean" && rhsType == "boolean") {
        return "boolean";
    }

    std::cerr << "Error: ";
    std::cerr << "(line " << lineno << ") ";
    std::cerr << type << " operation does not support operands of types ";
    std::cerr << lhsType << " and " << rhsType << ".\n";
    return "";
}

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
