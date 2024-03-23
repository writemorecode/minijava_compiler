#ifndef ARITHMETIC_EXPRESSION_NODE_H
#define ARITHMETIC_EXPRESSION_NODE_H

#include "Node.h"

class ArithmeticExpressionNode : public Node {
  protected:
    Node *left, *right;

  public:
    ArithmeticExpressionNode(const std::string &t, Node *left, Node *right,
                             int l)
        : Node(t, l, {left, right}), left{left}, right{right} {};

    std::string checkTypes(SymbolTable &st) const override;
};

class PlusNode : public ArithmeticExpressionNode {

  public:
    PlusNode(Node *left, Node *right, int l)
        : ArithmeticExpressionNode("Plus", left, right, l){};
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class MinusNode : public ArithmeticExpressionNode {

  public:
    MinusNode(Node *left, Node *right, int l)
        : ArithmeticExpressionNode("Minus", left, right, l){};
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class MultiplicationNode : public ArithmeticExpressionNode {

  public:
    MultiplicationNode(Node *left, Node *right, int l)
        : ArithmeticExpressionNode("Multiplication", left, right, l){};
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class DivisionNode : public ArithmeticExpressionNode {

  public:
    DivisionNode(Node *left, Node *right, int l)
        : ArithmeticExpressionNode("Division", left, right, l){};
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
