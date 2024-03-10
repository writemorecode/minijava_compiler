#ifndef METHOD_WITHOUT_PARAMETERS_NODE_H
#define METHOD_WITHOUT_PARAMETERS_NODE_H

#include "Node.h"

class MethodWithoutParametersNode : public Node {
    Node *type, *id, *body;

  public:
    MethodWithoutParametersNode(Node *type, Node *id, Node *body, int l)
        : Node("Method", l, {type, id, body}), type{type}, id{id}, body{
                                                                       body} {};

    bool buildTable(SymbolTable &st) const override;
    std::string checkTypes(SymbolTable &st) const override;
};

#endif
