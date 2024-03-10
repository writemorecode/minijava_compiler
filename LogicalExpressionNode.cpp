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
