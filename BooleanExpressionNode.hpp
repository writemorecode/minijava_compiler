#ifndef BOOLEAN_EXPRESSION_NODE_H
#define BOOLEAN_EXPRESSION_NODE_H

#include "Node.h"

class BooleanExpressionNode : public Node {
  protected:
    Node *left, *right;

  public:
    BooleanExpressionNode(const std::string &t, Node *left, Node *right, int l)
        : Node(t, l, {left, right}), left{left}, right{right} {};
    std::string checkTypes(SymbolTable &st) const override;
};

class AndNode : public BooleanExpressionNode {
  public:
    AndNode(Node *left, Node *right, int l)
        : BooleanExpressionNode("AND", left, right, l){};
    std::string generateIR(CFG &graph, SymbolTable &st) override;
};

class OrNode : public BooleanExpressionNode {
  public:
    OrNode(Node *left, Node *right, int l)
        : BooleanExpressionNode("OR", left, right, l){};
    std::string generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
