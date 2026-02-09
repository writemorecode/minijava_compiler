#ifndef BOOLEAN_EXPRESSION_NODE_H
#define BOOLEAN_EXPRESSION_NODE_H

#include "ast/Node.h"

class BooleanExpressionNode : public Node {
  protected:
    Node *left, *right;

  public:
    BooleanExpressionNode(const std::string &t, std::unique_ptr<Node> left_,
                          std::unique_ptr<Node> right_, int l)
        : Node(t, l) {
        left = append_child(std::move(left_));
        right = append_child(std::move(right_));
    }
};

class AndNode : public BooleanExpressionNode {
  public:
    AndNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right, int l)
        : BooleanExpressionNode("AND", std::move(left), std::move(right), l) {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class OrNode : public BooleanExpressionNode {
  public:
    OrNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right, int l)
        : BooleanExpressionNode("OR", std::move(left), std::move(right), l) {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
