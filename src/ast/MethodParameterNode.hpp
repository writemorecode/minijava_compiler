#ifndef METHOD_PARAMETER_NODE_H
#define METHOD_PARAMETER_NODE_H

#include "ast/Node.h"

class MethodParameterNode : public Node {
    Node *type, *id;

  public:
    MethodParameterNode(std::unique_ptr<Node> type_, std::unique_ptr<Node> id_,
                        int l)
        : Node("Method parameter", l) {
        type = append_child(std::move(type_));
        id = append_child(std::move(id_));
    }
    bool buildTable(SymbolTable &st) const override;
};

#endif
