#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "Node.h"

class ArrayAccessNode : public Node {
    Node *array, *index;

  public:
    ArrayAccessNode(Node *array, Node *index, int l)
        : Node("Array access", l, {array, index}), array{array},
          index{index} {};
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph) override;
};

class UnaryExpressionNode : public Node {
    Node *arg;

  public:
    UnaryExpressionNode(const std::string &t, Node *arg, int l)
        : Node(t, l, {arg}), arg{arg} {};
};

class ArrayLengthNode : public UnaryExpressionNode {
    Node *array;

  public:
    ArrayLengthNode(Node *array, int l)
        : UnaryExpressionNode("Array length", array, l), array{array} {};
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph) override;
};

class IntegerArrayAllocationNode : public UnaryExpressionNode {
    Node *length;

  public:
    IntegerArrayAllocationNode(Node *length, int l)
        : UnaryExpressionNode("Integer array allocation", length, l),
          length{length} {};
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph) override;
};

class NotNode : public UnaryExpressionNode {
    Node *expr;

  public:
    NotNode(Node *expr, int l)
        : UnaryExpressionNode("Negated expression", expr, l), expr{expr} {};
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph) override;
};

class EqualToNode : public Node {
    Node *left, *right;

  public:
    EqualToNode(Node *left, Node *right, int l)
        : Node("EQ", l, {left, right}), left{left}, right{right} {};
    std::string checkTypes(SymbolTable &st) const override;
};

class ThisNode : public Node {
    std::string value{"this"};

  public:
    ThisNode(int l) : Node("this", l) {}
    std::string checkTypes(SymbolTable &st) const override;
};

class BooleanNode : public Node {
    std::string value;

  public:
    BooleanNode(const std::string &value, int l)
        : Node(value, l), value{value} {}
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph) override;
};

class ClassAllocationNode : public Node {
    std::string id;

  public:
    ClassAllocationNode(Node *object, int l)
        : Node("Class allocation", object->value, l), id{object->value} {};
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph) override;
};

#endif
