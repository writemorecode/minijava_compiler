#ifndef ARITHMETIC_EXPRESSION_NODE_H
#define ARITHMETIC_EXPRESSION_NODE_H

#include "ast/Node.h"

class ArithmeticExpressionNode : public Node {
  protected:
    Node *left, *right;

  public:
    ArithmeticExpressionNode(const std::string &t, std::unique_ptr<Node> left_,
                             std::unique_ptr<Node> right_, int l)
        : Node(t, l) {
        left = append_child(std::move(left_));
        right = append_child(std::move(right_));
    }
};

class PlusNode : public ArithmeticExpressionNode {

  public:
    PlusNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right, int l)
        : ArithmeticExpressionNode("Plus", std::move(left), std::move(right),
                                   l) {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class MinusNode : public ArithmeticExpressionNode {

  public:
    MinusNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right, int l)
        : ArithmeticExpressionNode("Minus", std::move(left), std::move(right),
                                   l) {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class MultiplicationNode : public ArithmeticExpressionNode {

  public:
    MultiplicationNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right,
                       int l)
        : ArithmeticExpressionNode("Multiplication", std::move(left),
                                   std::move(right), l) {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class DivisionNode : public ArithmeticExpressionNode {

  public:
    DivisionNode(std::unique_ptr<Node> left, std::unique_ptr<Node> right, int l)
        : ArithmeticExpressionNode("Division", std::move(left),
                                   std::move(right), l) {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
