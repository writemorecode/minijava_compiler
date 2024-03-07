#ifndef VARIABLE_NODE_H
#define VARIABLE_NODE_H

#include "Node.h"

class VariableNode : public Node {
    Node *type, *name;

public:
    VariableNode(Node* type, Node* name, int l)
        : Node("Variable", l, { type, name })
        , type { type }
        , name { name } {};

    bool buildTable(SymbolTable& st) const override;
    std::string checkTypes(SymbolTable& st) const override;
};

#endif
