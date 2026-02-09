#ifndef CLASS_NODE_H
#define CLASS_NODE_H

#include "ast/Node.h"

class ClassNode : public Node {
    Node *id, *body;
    std::string className;

  public:
    ClassNode(std::unique_ptr<Node> id_, std::unique_ptr<Node> body_, int l)
        : Node("Class", l) {
        id = append_child(std::move(id_));
        body = append_child(std::move(body_));
        className = id->value;
    }
    void accept(AstVisitor &visitor) const override;

    [[nodiscard]] const std::string &getClassName() const { return className; }
    [[nodiscard]] const Node &getBodyNode() const { return *body; }

    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
