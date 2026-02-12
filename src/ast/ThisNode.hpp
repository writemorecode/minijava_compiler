#ifndef THISNODE_HPP
#define THISNODE_HPP

#include "ast/Node.h"

class ThisNode : public Node {
    std::string value{"this"};

  public:
    ThisNode(int l) : Node("this", l) {}
};

#endif // THISNODE_HPP
