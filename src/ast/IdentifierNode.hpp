#ifndef IDENTIFIER_NODE_H
#define IDENTIFIER_NODE_H

#include "ir/BBlock.hpp"
#include "ast/Node.h"

class IdentifierNode : public Node {
    std::string value;

  public:
    IdentifierNode(const std::string &value_, int l)
        : Node("Identifier", value_, l), value{value_} {}

    std::string checkTypes(SymbolTable &st) const override;

    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
