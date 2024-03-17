#include "IntegerNode.hpp"

std::string IntegerNode::checkTypes(SymbolTable &st) const { return "int"; }

std::string IntegerNode::generateIR(CFG &graph, SymbolTable &st) {
    return std::to_string(value);
}
