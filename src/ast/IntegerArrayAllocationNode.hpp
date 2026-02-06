#ifndef INTEGERARRAYALLOCATIONNODE_HPP
#define INTEGERARRAYALLOCATIONNODE_HPP

#include "ast/Node.h"

class IntegerArrayAllocationNode : public Node {
    std::unique_ptr<Node> length;

  public:
    IntegerArrayAllocationNode(std::unique_ptr<Node> length_, int l)
        : Node("Integer array allocation", l), length(std::move(length_)) {}
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // INTEGERARRAYALLOCATIONNODE_HPP
