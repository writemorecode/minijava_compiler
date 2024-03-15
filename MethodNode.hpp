#ifndef METHOD_NODE_H
#define METHOD_NODE_H

#include "Node.h"

class MethodNode : public Node {
    Node *type, *id, *params, *body;

  public:
    MethodNode(Node *type, Node *id, Node *params, Node *body, int l)
        : Node("Method", l, {type, id, params, body}), type{type}, id{id},
          params{params}, body{body} {};
    bool buildTable(SymbolTable &st) const override;
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph) override;
};

#endif
