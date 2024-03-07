#ifndef METHOD_PARAMETER_NODE_H
#define METHOD_PARAMETER_NODE_H

#include "Node.h"

class MethodParameterNode : public Node {
    Node *type, *id;

public:
    MethodParameterNode(Node* type, Node* id, int l)
        : Node("Method parameter", l, { type, id })
        , type { type }
        , id { id }
    {
    }
    bool buildTable(SymbolTable& st) const override;
};

#endif
