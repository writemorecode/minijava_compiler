#ifndef NOTNODE_HPP
#define NOTNODE_HPP

#include "ast/Node.h"

class NotNode : public Node {
    Node *expr;

  public:
    NotNode(std::unique_ptr<Node> expr_, int l)
        : Node("Negated expression", l) {
        expr = append_child(std::move(expr_));
    }
};

#endif // NOTNODE_HPP
