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
std::string BooleanNode::checkTypes(SymbolTable &st) const { return "boolean"; }
std::string BooleanNode::generateIR(CFG &graph) { return value; }
std::string ClassAllocationNode::checkTypes(SymbolTable &st) const {
    return id;
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
