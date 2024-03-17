#ifndef INTEGER_NODE_H
#define INTEGER_NODE_H

#include <charconv>
#include <string>

#include "Node.h"

class IntegerNode : public Node {
    int value;

  public:
    IntegerNode(const std::string &value_, int l)
        : Node("Integer", value_, l), value{std::stoi(value_)} {}

    std::string checkTypes(SymbolTable &st) const override;

    std::string generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
