#ifndef METHOD_NODE_H
#define METHOD_NODE_H

#include "ast/Node.h"

class MethodNode : public Node {
    Node *type, *id, *params, *body;
    std::string methodName, methodType;

  public:
    MethodNode(Node *type_, Node *id_, Node *params_, Node *body_, int l)
        : Node("Method", l, {type_, id_, params_, body_}), type{type_}, id{id_},
          params{params_}, body{body_}, methodName{id->value},
          methodType{type->value} {};
    bool buildTable(SymbolTable &st) const override;
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
