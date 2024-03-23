#ifndef STATEMENT_NODE_H
#define STATEMENT_NODE_H

#include "Node.h"

class AssignNode : public Node {
    Node *id;
    Node *expr;

  public:
    AssignNode(Node *id, Node *expr, int l)
        : Node("Assign", l, {id, expr}), id{id}, expr{expr} {}
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class ArrayAssignNode : public Node {
    Node *id;
    Node *indexExpr;
    Node *rightExpr;

  public:
    ArrayAssignNode(Node *id, Node *indexExpr, Node *rightExpr, int l)
        : Node("Array assign", l, {id, indexExpr}), id{id},
          indexExpr{indexExpr}, rightExpr{rightExpr} {}
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class PrintNode : public Node {
    Node *expr;

  public:
    PrintNode(Node *expr, int l) : Node("Print", l, {expr}), expr{expr} {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
