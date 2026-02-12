#ifndef BOOLEANNODE_HPP
#define BOOLEANNODE_HPP

#include "ast/Node.h"

class TrueNode : public Node {
  public:
    TrueNode(int l) : Node("TRUE", l) {};
};
class FalseNode : public Node {
  public:
    FalseNode(int l) : Node("FALSE", l) {};
};

#endif // BOOLEANNODE_HPP
