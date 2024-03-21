#include "BooleanNode.hpp"
#include "Tac.hpp"

std::string BooleanNode::checkTypes(SymbolTable &st) const { return "boolean"; }

std::string BooleanNode::generateIR(CFG &graph, SymbolTable &st) {
    return value;
}
