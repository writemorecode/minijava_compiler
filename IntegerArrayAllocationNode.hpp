#ifndef INTEGERARRAYALLOCATIONNODE_HPP
#define INTEGERARRAYALLOCATIONNODE_HPP

#include "ExpressionNode.hpp"
#include "Node.h"

class IntegerArrayAllocationNode : public UnaryExpressionNode {
    Node *length;

  public:
    IntegerArrayAllocationNode(Node *length, int l)
        : UnaryExpressionNode("Integer array allocation", length, l),
          length{length} {};
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // INTEGERARRAYALLOCATIONNODE_HPP
