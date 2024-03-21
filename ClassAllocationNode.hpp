#ifndef CLASSALLOCATIONNODE_HPP
#define CLASSALLOCATIONNODE_HPP

#include "Node.h"

class ClassAllocationNode : public Node {
    std::string id;

  public:
    ClassAllocationNode(Node *object, int l)
        : Node("Class allocation", object->value, l), id{object->value} {};
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph, SymbolTable &st) override;
};

#endif // CLASSALLOCATIONNODE_HPP
