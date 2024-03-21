#ifndef ARRAYLENGTHNODE_HPP
#define ARRAYLENGTHNODE_HPP

#include "ExpressionNode.hpp"
#include "Node.h"

class ArrayLengthNode : public UnaryExpressionNode {
    Node *array;

  public:
    ArrayLengthNode(Node *array, int l)
        : UnaryExpressionNode("Array length", array, l), array{array} {};
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // ARRAYLENGTHNODE_HPP
