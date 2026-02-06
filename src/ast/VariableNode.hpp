#ifndef VARIABLE_NODE_H
#define VARIABLE_NODE_H

#include "ast/Node.h"

class VariableNode : public Node {
    Node *type, *name;

  public:
    VariableNode(std::unique_ptr<Node> type_, std::unique_ptr<Node> name_,
                 int l)
        : Node("Variable", l) {
        type = append_child(std::move(type_));
        name = append_child(std::move(name_));
    }

    bool buildTable(SymbolTable &st) const override;
    std::string checkTypes(SymbolTable &st) const override;
};

#endif
