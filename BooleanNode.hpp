#ifndef BOOLEANNODE_HPP
#define BOOLEANNODE_HPP

#include "Node.h"

class BooleanNode : public Node {
    std::string value;

  public:
    BooleanNode(const std::string &value, int l)
        : Node(value, l), value{value} {}
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // BOOLEANNODE_HPP
