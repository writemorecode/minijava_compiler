#ifndef NOTNODE_HPP
#define NOTNODE_HPP

#include "ExpressionNode.hpp"
#include "Node.h"

class NotNode : public UnaryExpressionNode {
    Node *expr;

  public:
    NotNode(Node *expr, int l)
        : UnaryExpressionNode("Negated expression", expr, l), expr{expr} {};
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // NOTNODE_HPP
