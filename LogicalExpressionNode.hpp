#ifndef LOGICAL_EXPRESSION_NODE_H
#define LOGICAL_EXPRESSION_NODE_H

#include "Node.h"

class LogicalExpressionNode : public Node {
  protected:
    Node *left, *right;

  public:
    LogicalExpressionNode(const std::string &t, Node *left, Node *right, int l)
        : Node(t, l, {left, right}), left{left}, right{right} {};

    std::string checkTypes(SymbolTable &st) const override;
};

class LessThanNode : public LogicalExpressionNode {
  public:
    LessThanNode(Node *left, Node *right, int l)
        : LogicalExpressionNode("Less-than", left, right, l){};
    std::string generateIR(CFG &graph) override;
};

class GreaterThanNode : public LogicalExpressionNode {
  public:
    GreaterThanNode(Node *left, Node *right, int l)
        : LogicalExpressionNode("Greater-than", left, right, l){};
    std::string generateIR(CFG &graph) override;
};

#endif
