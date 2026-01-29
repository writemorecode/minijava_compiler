#ifndef CLASS_NODE_H
#define CLASS_NODE_H

#include "ast/Node.h"

class ClassNode : public Node {
    Node *id, *body;
    std::string className;

  public:
    ClassNode(Node *id_, Node *body_, int l)
        : Node("Class", l, {id_, body_}), id{id_}, body{body_},
          className{id->value} {};
    bool buildTable(SymbolTable &st) const override;
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
