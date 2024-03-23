#include "IntegerNode.hpp"

std::string IntegerNode::checkTypes(SymbolTable &st) const { return "int"; }

Operand IntegerNode::generateIR(CFG &graph, SymbolTable &st) { return value; }
