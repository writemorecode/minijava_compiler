#ifndef CLASSALLOCATIONNODE_HPP
#define CLASSALLOCATIONNODE_HPP

#include "ast/Node.h"

class ClassAllocationNode : public Node {
    std::string id;

  public:
    ClassAllocationNode(std::unique_ptr<Node> object, int l)
        : Node("Class allocation", object->value, l), id{object->value} {}
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // CLASSALLOCATIONNODE_HPP
