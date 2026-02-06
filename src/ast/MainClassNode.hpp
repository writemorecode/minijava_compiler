#ifndef MAIN_CLASS_NODE_H
#define MAIN_CLASS_NODE_H

#include "ast/Node.h"

class MainClassNode : public Node {
    Node *id, *arg, *body;
    std::string mainClassName, mainMethodArgumentName;

  public:
    MainClassNode(std::unique_ptr<Node> id_, std::unique_ptr<Node> arg_,
                  std::unique_ptr<Node> body_, int l)
        : Node("Main Class", l) {
        id = append_child(std::move(id_));
        arg = append_child(std::move(arg_));
        body = append_child(std::move(body_));
        mainClassName = id->value;
        mainMethodArgumentName = arg->value;
    }

    bool buildTable(SymbolTable &st) const override;
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
