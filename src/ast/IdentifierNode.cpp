#include "ast/IdentifierNode.hpp"

Operand IdentifierNode::generateIR(CFG &graph, SymbolTable &st) {
    return value;
}
