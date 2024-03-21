#ifndef EXPRESSION_NODE_H
#define EXPRESSION_NODE_H

#include "Node.h"

class UnaryExpressionNode : public Node {
  public:
    UnaryExpressionNode(const std::string &t, Node *arg_, int l)
        : Node(t, l, {arg_}){};
};

#endif
