#include "StatementNode.hpp"

std::string AssignNode::checkTypes(SymbolTable &st) const {
    const auto lhsType = id->checkTypes(st);
    const auto rhsType = expr->checkTypes(st);
    if (lhsType == rhsType) {
        return lhsType;
    }
    if (lhsType != "" && rhsType != "") {
        std::cerr << "Error: (line " << lineno << ") Cannot assign type '"
                  << rhsType << "' to type '" << lhsType << "'.\n";
    }
    return "";
}

std::string AssignNode::generateIR(CFG &graph) {
    auto rhsName = expr->generateIR(graph);
    auto lhsName = id->value;
    graph.addInstruction(new CopyTac(rhsName, lhsName));
    return lhsName;
}

std::string ArrayAssignNode::checkTypes(SymbolTable &st) const {
    const auto indexType = indexExpr->checkTypes(st);

    if (indexType != "int") {
        std::cerr << "Error: (line " << lineno << ") ";
        std::cerr << "Invalid array index type ";
        std::cerr << "'" << indexType << "', ";
        std::cerr << "expected type 'int'.\n";
        return "";
    }

    const auto lhsType = id->checkTypes(st);
    if (lhsType != "int[]") {
        std::cerr << "Error: (line " << lineno
                  << ") Invalid array type "
                     "'"
                  << lhsType << "', expected type 'int[]'.\n";
        return "";
    }

    const auto rhsType = rightExpr->checkTypes(st);
    if (!rhsType.empty() && rhsType != "int") {
        std::cerr << "Error: (line " << lineno
                  << ") Cannot assign value of type "
                     "'"
                  << rhsType << "', to array of type '" << lhsType << "'.\n";
        return "";
    }

    return "";
}
