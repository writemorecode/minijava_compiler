#include "ast/ThisNode.hpp"
#include "ir/Tac.hpp"

Operand ThisNode::generateIR(CFG &graph, SymbolTable &st) { return value; }
