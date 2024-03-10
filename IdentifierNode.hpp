#ifndef IDENTIFIER_NODE_H
#define IDENTIFIER_NODE_H

#include "BBlock.hpp"
#include "Node.h"

class IdentifierNode : public Node {
    std::string value;

  public:
    IdentifierNode(const std::string &value_, int l)
        : Node("Identifier", value_, l), value{value_} {}

    std::string checkTypes(SymbolTable &st) const override;

    std::string generateIR(CFG &graph) override;
};

#endif
