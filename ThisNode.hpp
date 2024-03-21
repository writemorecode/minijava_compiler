#ifndef THISNODE_HPP
#define THISNODE_HPP

#include "Node.h"

class ThisNode : public Node {
    std::string value{"this"};

  public:
    ThisNode(int l) : Node("this", l) {}
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // THISNODE_HPP
