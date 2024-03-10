#ifndef METHOD_CALL_WITHOUT_ARGUMENTS_NODE_HPP
#define METHOD_CALL_WITHOUT_ARGUMENTS_NODE_HPP

#include "Node.h"
class MethodCallWithoutArgumentsNode : public Node {
    Node *object, *id;

  public:
    MethodCallWithoutArgumentsNode(Node *object, Node *id, int l)
        : Node("Method call", l, {object, id}), object{object}, id{id} {};
    std::string checkTypes(SymbolTable &st) const override;
};

#endif
