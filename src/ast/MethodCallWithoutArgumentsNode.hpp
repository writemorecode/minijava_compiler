#ifndef METHOD_CALL_WITHOUT_ARGUMENTS_NODE_HPP
#define METHOD_CALL_WITHOUT_ARGUMENTS_NODE_HPP

#include "ast/Node.h"
class MethodCallWithoutArgumentsNode : public Node {
    Node *object, *id;

  public:
    MethodCallWithoutArgumentsNode(std::unique_ptr<Node> object_,
                                   std::unique_ptr<Node> id_, int l)
        : Node("Method call", l) {
        object = append_child(std::move(object_));
        id = append_child(std::move(id_));
    }
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
