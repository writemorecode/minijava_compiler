#ifndef METHOD_NODE_H
#define METHOD_NODE_H

#include "ast/Node.h"

class MethodNode : public Node {
    Node *type, *id, *params, *body;
    std::string methodName, methodType;

  public:
    MethodNode(std::unique_ptr<Node> type_, std::unique_ptr<Node> id_,
               std::unique_ptr<Node> params_, std::unique_ptr<Node> body_,
               int l)
        : Node("Method", l) {
        type = append_child(std::move(type_));
        id = append_child(std::move(id_));
        params = append_child(std::move(params_));
        body = append_child(std::move(body_));
        methodName = id->value;
        methodType = type->value;
    }

    void accept(AstVisitor &visitor) const override;

    [[nodiscard]] const std::string &getMethodName() const { return methodName; }
    [[nodiscard]] const std::string &getMethodType() const { return methodType; }
    [[nodiscard]] const Node &getParametersNode() const { return *params; }
    [[nodiscard]] const Node &getBodyNode() const { return *body; }

    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
