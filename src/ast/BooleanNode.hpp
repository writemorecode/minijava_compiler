#ifndef BOOLEANNODE_HPP
#define BOOLEANNODE_HPP

#include "ast/Node.h"

class TrueNode : public Node {
  public:
    TrueNode(int l) : Node("TRUE", l) {};
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};
class FalseNode : public Node {
  public:
    FalseNode(int l) : Node("FALSE", l) {};
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // BOOLEANNODE_HPP
