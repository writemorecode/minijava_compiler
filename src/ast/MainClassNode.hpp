#ifndef MAIN_CLASS_NODE_H
#define MAIN_CLASS_NODE_H

#include "ast/Node.h"

class MainClassNode : public Node {
    Node *id, *arg, *body;
    std::string mainClassName, mainMethodArgumentName;

  public:
    MainClassNode(Node *id_, Node *arg_, Node *body_, int l)
        : Node("Main Class", l, {id_, arg_, body_}), id{id_}, arg{arg_},
          body{body_}, mainClassName{id->value},
          mainMethodArgumentName{arg->value} {};

    bool buildTable(SymbolTable &st) const override;
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
