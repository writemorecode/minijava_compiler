#include "NotNode.hpp"
#include "Tac.hpp"

std::string NotNode::checkTypes(SymbolTable &st) const {
    const auto exprType = expr->checkTypes(st);
    if (exprType == "boolean") {
        return "boolean";
    }
    std::cerr << "Error: ";
    std::cerr << "(line " << lineno << ") ";
    std::cerr << "Invalid type '" << exprType
              << "' for negation operator, expected type 'boolean'.\n";
    return "";
}

std::string NotNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    auto rhsName = expr->generateIR(graph, st);
    graph.addInstruction(new NotTac(name, rhsName));
    return name;
}
