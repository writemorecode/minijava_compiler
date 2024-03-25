#ifndef BOOLEANNODE_HPP
#define BOOLEANNODE_HPP

#include "Node.h"

class TrueNode : public Node {
  public:
    TrueNode(int l) : Node("TRUE", l){};
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};
class FalseNode : public Node {
  public:
    FalseNode(int l) : Node("FALSE", l){};
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // BOOLEANNODE_HPP
