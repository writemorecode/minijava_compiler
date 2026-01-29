#ifndef TYPE_NODE_H
#define TYPE_NODE_H

#include "ast/Node.h"

class TypeNode : public Node {
    std::string value;

  public:
    TypeNode(const std::string &value_, int l)
        : Node("Type", value_, l), value{value_} {}

    std::string checkTypes(SymbolTable &st) const override;
};

#endif
