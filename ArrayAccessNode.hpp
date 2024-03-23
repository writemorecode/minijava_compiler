#ifndef ARRAYACCESSNODE_HPP
#define ARRAYACCESSNODE_HPP

#include "Node.h"

class ArrayAccessNode : public Node {
    Node *array, *index;

  public:
    ArrayAccessNode(Node *array, Node *index, int l)
        : Node("Array access", l, {array, index}), array{array},
          index{index} {};
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // ARRAYACCESSNODE_HPP
