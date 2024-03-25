#include "BooleanNode.hpp"
#include "Tac.hpp"

std::string TrueNode::checkTypes(SymbolTable &st) const { return "boolean"; }
std::string FalseNode::checkTypes(SymbolTable &st) const { return "boolean"; }
Operand TrueNode::generateIR(CFG &graph, SymbolTable &st) { return true; }
Operand FalseNode::generateIR(CFG &graph, SymbolTable &st) { return false; }
