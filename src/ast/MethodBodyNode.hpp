#ifndef METHOD_BODY_NODE_H
#define METHOD_BODY_NODE_H

#include "ast/Node.h"

class MethodBodyNode : public Node {
    Node *body, *returnValue;

  public:
    MethodBodyNode(std::unique_ptr<Node> body_,
                   std::unique_ptr<Node> returnValue_, int l)
        : Node("Method body", l) {
        body = append_child(std::move(body_));
        returnValue = append_child(std::move(returnValue_));
    }
};
class ReturnOnlyMethodBodyNode : public Node {
    Node *returnValue;

  public:
    ReturnOnlyMethodBodyNode(std::unique_ptr<Node> returnValue_, int l)
        : Node("Method body", l) {
        returnValue = append_child(std::move(returnValue_));
    }
};
#endif
