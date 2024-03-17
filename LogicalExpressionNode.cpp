#include "LogicalExpressionNode.hpp"

std::string LogicalExpressionNode::checkTypes(SymbolTable &st) const {
    const auto lhsType = left->checkTypes(st);
    const auto rhsType = right->checkTypes(st);

    if (lhsType == "int" && rhsType == "int") {
        return "boolean";
    }

    if (!lhsType.empty() && !rhsType.empty()) {
        std::cerr << "Error: (line " << lineno << ") " << type << " "
                  << "operation does not "
                     "support operands of types '"
                  << lhsType
                  << "' and "
                     "'"
                  << rhsType << "'.\n";
    }
    return "";
}

std::string LessThanNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    graph.addInstruction(new ExpressionTac(name, lhs_name, "<", rhs_name));
    return name;
}
std::string GreaterThanNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    auto lhs_name = left->generateIR(graph, st);
    auto rhs_name = right->generateIR(graph, st);
    graph.addInstruction(new ExpressionTac(name, lhs_name, ">", rhs_name));
    return name;
}
