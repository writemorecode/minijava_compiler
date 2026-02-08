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

    void accept(AstVisitor &visitor) const override;

    [[nodiscard]] const std::string &getMainClassName() const {
        return mainClassName;
    }
    [[nodiscard]] const std::string &getMainMethodArgumentName() const {
        return mainMethodArgumentName;
    }
    [[nodiscard]] const Node &getBodyNode() const { return *body; }

    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
