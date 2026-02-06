#ifndef METHOD_WITHOUT_PARAMETERS_NODE_H
#define METHOD_WITHOUT_PARAMETERS_NODE_H

#include "ast/Node.h"

class MethodWithoutParametersNode : public Node {
    Node *type, *id, *body;

  public:
    MethodWithoutParametersNode(std::unique_ptr<Node> type_,
                                std::unique_ptr<Node> id_,
                                std::unique_ptr<Node> body_, int l)
        : Node("Method", l) {
        type = append_child(std::move(type_));
        id = append_child(std::move(id_));
        body = append_child(std::move(body_));
    }

    bool buildTable(SymbolTable &st) const override;
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
