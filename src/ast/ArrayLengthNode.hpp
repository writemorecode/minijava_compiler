#ifndef ARRAYLENGTHNODE_HPP
#define ARRAYLENGTHNODE_HPP

#include "ast/Node.h"

class ArrayLengthNode : public Node {
    std::unique_ptr<Node> array;

  public:
    ArrayLengthNode(std::unique_ptr<Node> array_, int l)
        : Node("Array length", l), array(std::move(array_)) {}
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // ARRAYLENGTHNODE_HPP
