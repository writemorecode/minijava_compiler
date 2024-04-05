#ifndef INTEGERARRAYALLOCATIONNODE_HPP
#define INTEGERARRAYALLOCATIONNODE_HPP

#include "Node.h"

class IntegerArrayAllocationNode : public Node {
    Node *length;

  public:
    IntegerArrayAllocationNode(Node *length, int l)
        : Node("Integer array allocation", l), length{length} {};
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // INTEGERARRAYALLOCATIONNODE_HPP
