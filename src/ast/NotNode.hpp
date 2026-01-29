#ifndef NOTNODE_HPP
#define NOTNODE_HPP

#include "ast/Node.h"

class NotNode : public Node {
    Node *expr;

  public:
    NotNode(Node *expr, int l)
        : Node("Negated expression", l, {expr}), expr{expr} {};
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // NOTNODE_HPP
