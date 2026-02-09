#ifndef STATEMENT_NODE_H
#define STATEMENT_NODE_H

#include "ast/Node.h"

class AssignNode : public Node {
    Node *id;
    Node *expr;

  public:
    AssignNode(std::unique_ptr<Node> id_, std::unique_ptr<Node> expr_, int l)
        : Node("Assign", l) {
        id = append_child(std::move(id_));
        expr = append_child(std::move(expr_));
    }
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class ArrayAssignNode : public Node {
    Node *id;
    Node *indexExpr;
    std::unique_ptr<Node> rightExpr;

  public:
    ArrayAssignNode(std::unique_ptr<Node> id_, std::unique_ptr<Node> indexExpr_,
                    std::unique_ptr<Node> rightExpr_, int l)
        : Node("Array assign", l), rightExpr(std::move(rightExpr_)) {
        id = append_child(std::move(id_));
        indexExpr = append_child(std::move(indexExpr_));
    }
    [[nodiscard]] const Node &getRightExprNode() const { return *rightExpr; }
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

class PrintNode : public Node {
    Node *expr;

  public:
    PrintNode(std::unique_ptr<Node> expr_, int l) : Node("Print", l) {
        expr = append_child(std::move(expr_));
    }
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
