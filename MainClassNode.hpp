#ifndef MAIN_CLASS_NODE_H
#define MAIN_CLASS_NODE_H

#include "Node.h"

class MainClassNode : public Node {
    Node *id, *arg, *body;

  public:
    MainClassNode(Node *id, Node *arg, Node *body, int l)
        : Node("Main Class", l, {id, arg, body}), id{id}, arg{arg},
          body{body} {};

    bool buildTable(SymbolTable &st) const override;
    std::string checkTypes(SymbolTable &st) const override;
};

#endif
