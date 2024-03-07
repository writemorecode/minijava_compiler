#ifndef CLASS_NODE_H
#define CLASS_NODE_H

#include "Node.h"

class ClassNode : public Node {
    Node *id, *body;

public:
    ClassNode(Node* id, Node* body, int l)
        : Node("Class", l, { id, body })
        , id { id }
        , body { body } {};
    bool buildTable(SymbolTable& st) const override;
    std::string checkTypes(SymbolTable& st) const override;
};

#endif
