#ifndef ARRAYLENGTHNODE_HPP
#define ARRAYLENGTHNODE_HPP

#include "Node.h"

class ArrayLengthNode : public Node {
    Node *array;

  public:
    ArrayLengthNode(Node *array, int l)
        : Node("Array length", l), array{array} {};
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // ARRAYLENGTHNODE_HPP
