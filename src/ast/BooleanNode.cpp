#include "ast/BooleanNode.hpp"
#include "ir/Tac.hpp"
Operand TrueNode::generateIR(CFG &graph, SymbolTable &st) { return true; }
Operand FalseNode::generateIR(CFG &graph, SymbolTable &st) { return false; }
