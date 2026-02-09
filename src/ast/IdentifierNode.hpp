#ifndef IDENTIFIER_NODE_H
#define IDENTIFIER_NODE_H

#include "ast/Node.h"
#include "ir/BBlock.hpp"

class IdentifierNode : public Node {
    std::string value;

  public:
    IdentifierNode(const std::string &value_, int l)
        : Node("Identifier", value_, l), value{value_} {}

    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
