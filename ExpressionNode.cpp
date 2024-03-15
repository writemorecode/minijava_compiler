#include "ExpressionNode.hpp"

std::string ThisNode::checkTypes(SymbolTable &st) const {
    const auto lookup = st.lookupVariable(type);
    if (!lookup) {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Undeclared identifier (type " << type << ")" << value
                  << ".\n";
        return "";
    }
    return lookup->getType();
}
std::string ThisNode::generateIR(CFG &graph) { return value; }
std::string BooleanNode::checkTypes(SymbolTable &st) const { return "boolean"; }
std::string BooleanNode::generateIR(CFG &graph) { return value; }
std::string ClassAllocationNode::checkTypes(SymbolTable &st) const {
    return id;
}
std::string ClassAllocationNode::generateIR(CFG &graph) {
    auto name = graph.getTemporaryName();
    graph.addInstruction(new NewTac(name, id));
    return name;
}
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

std::string NotNode::generateIR(CFG &graph) {
    auto name = graph.getTemporaryName();
    auto rhsName = expr->generateIR(graph);
    graph.addInstruction(new UnaryExpressionTac(name, "!", rhsName));
    return name;
}

std::string ArrayAccessNode::checkTypes(SymbolTable &st) const {
    const auto arrayType = array->checkTypes(st);
    const auto indexType = index->checkTypes(st);

    if (indexType != "int") {
        std::cerr << "Error: (line " << lineno << ") ";
        std::cerr << "Invalid array index type ";
        std::cerr << "'" << indexType << "', ";
        std::cerr << "expected type 'int'.\n";
        return "";
    }
    if (arrayType != "int[]") {
        std::cerr << "Error: (line " << lineno << ") ";
        std::cerr << "Invalid array type ";
        std::cerr << "'" << indexType << "', ";
        std::cerr << "expected type 'int[]'.\n";
        return "";
    }

    return "int";
}

std::string ArrayAccessNode::generateIR(CFG &graph) {
    auto indexName = index->generateIR(graph);
    auto arrayName = array->value;
    auto name = graph.getTemporaryName();
    graph.addInstruction(new ArrayAccessTac(name, arrayName, indexName));
    return name;
}

std::string IntegerArrayAllocationNode::checkTypes(SymbolTable &st) const {
    const auto lengthType = length->checkTypes(st);
    if (lengthType != "int") {
        std::cerr << "Error: (line " << lineno << ") Invalid type '"
                  << lengthType
                  << "' for array length"
                     ", expected type 'int'.\n";
        return "";
    }
    return "int[]";
}
std::string IntegerArrayAllocationNode::generateIR(CFG &graph) {
    auto name = graph.getTemporaryName();
    auto lengthName = length->generateIR(graph);
    graph.addInstruction(new NewArrayTac(name, lengthName));
    return name;
}
std::string ArrayLengthNode::checkTypes(SymbolTable &st) const {
    const auto arrayType = array->checkTypes(st);
    if (arrayType != "int[]") {
        std::cerr << "Error: (line " << lineno << ") Invalid type '"
                  << arrayType
                  << "' for array length, "
                     "expected type 'int[]'.\n";
        return "";
    }
    return "int";
}

std::string ArrayLengthNode::generateIR(CFG &graph) {
    auto name = graph.getTemporaryName();
    auto arrayName = array->value;
    graph.addInstruction(new UnaryExpressionTac(name, "length", arrayName));
    return name;
}

std::string EqualToNode::checkTypes(SymbolTable &st) const {
    const auto lhsType = left->checkTypes(st);
    const auto rhsType = right->checkTypes(st);

    if ((lhsType == "boolean" && rhsType == "boolean") ||
        (lhsType == "int" && rhsType == "int")) {
        return "boolean";
    }
    std::cerr << "Error: ";
    std::cerr << "(line " << lineno << ") ";
    std::cerr << "Operator '==' does not support operands of types ";
    std::cerr << "'" << lhsType << "' and '" << rhsType << "'.\n";
    return "";
}

std::string EqualToNode::generateIR(CFG &graph) {
    auto name = graph.getTemporaryName();
    auto lhs_name = left->generateIR(graph);
    auto rhs_name = right->generateIR(graph);
    graph.addInstruction(new ExpressionTac(name, lhs_name, "==", rhs_name));
    return name;
}
