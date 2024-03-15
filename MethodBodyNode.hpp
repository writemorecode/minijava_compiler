#ifndef METHOD_BODY_NODE_H
#define METHOD_BODY_NODE_H

#include "Node.h"

class MethodBodyNode : public Node {
    Node *body, *returnValue;

  public:
    MethodBodyNode(Node *body, Node *returnValue, int l)
        : Node("Method body", l, {body, returnValue}), body{body},
          returnValue{returnValue} {}
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph) override;
};
class ReturnOnlyMethodBodyNode : public Node {
    Node *returnValue;

  public:
    ReturnOnlyMethodBodyNode(Node *returnValue, int l)
        : Node("Method body", l, {returnValue}), returnValue{returnValue} {}
    std::string checkTypes(SymbolTable &st) const override;
    std::string generateIR(CFG &graph) override;
};
#endif
