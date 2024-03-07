#include "ArithmeticExpressionNode.hpp"

std::string ArithmeticExpressionNode::checkTypes(SymbolTable& st) const
{
    const auto lhsType = left->checkTypes(st);
    const auto rhsType = right->checkTypes(st);

    if (lhsType == "int" && rhsType == "int") {
        return "int";
    }
    std::cerr << "Error: ";
    std::cerr << "(line " << lineno << ") ";
    std::cerr << type << " operation does not support operands of types ";
    std::cerr << lhsType << " and " << rhsType << ".\n";
    return "";
}
