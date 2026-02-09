#ifndef LOGICAL_EXPRESSION_NODE_H
#define LOGICAL_EXPRESSION_NODE_H

#include "ast/Node.h"

class LogicalExpressionNode : public Node {
  protected:
    Node *left, *right;

  public:
    LogicalExpressionNode(const std::string &t, std::unique_ptr<Node> left_,
                          std::unique_ptr<Node> right_, int l)
        : Node(t, l) {
        left = append_child(std::move(left_));
        right = append_child(std::move(right_));
    }
};

class LessThanNode : public LogicalExpressionNode {
  public:
    LessThanNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right, int l)
        : LogicalExpressionNode("Less-than", std::move(left), std::move(right),
                                l) {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class GreaterThanNode : public LogicalExpressionNode {
  public:
    GreaterThanNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right,
                    int l)
        : LogicalExpressionNode("Greater-than", std::move(left),
                                std::move(right), l) {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class EqualToNode : public Node {
    Node *left, *right;

  public:
    EqualToNode(std::unique_ptr<Node> left_, std::unique_ptr<Node> right_,
                int l)
        : Node("EQ", l) {
        left = append_child(std::move(left_));
        right = append_child(std::move(right_));
    }
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
