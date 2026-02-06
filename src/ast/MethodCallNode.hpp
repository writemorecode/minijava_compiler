#ifndef METHOD_CALL_NODE_HPP
#define METHOD_CALL_NODE_HPP

#include "ast/Node.h"

class MethodCallNode : public Node {
    Node *object;
    Node *id;
    Node *exprList;

  public:
    MethodCallNode(std::unique_ptr<Node> object_, std::unique_ptr<Node> id_,
                   std::unique_ptr<Node> exprList_, int l)
        : Node("Method call", l) {
        object = append_child(std::move(object_));
        id = append_child(std::move(id_));
        exprList = append_child(std::move(exprList_));
    }
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
