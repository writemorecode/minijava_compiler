#ifndef INTEGER_NODE_H
#define INTEGER_NODE_H

#include <charconv>
#include <string>

#include "ast/Node.h"

class IntegerNode : public Node {
    int value;

  public:
    IntegerNode(const std::string &value_, int l)
        : Node("Integer", value_, l), value{std::stoi(value_)} {}
};

#endif
