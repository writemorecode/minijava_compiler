#include "BooleanNode.hpp"
#include "Tac.hpp"

std::string BooleanNode::checkTypes(SymbolTable &st) const { return "boolean"; }

Operand BooleanNode::generateIR(CFG &graph, SymbolTable &st) {
    return value;
}
