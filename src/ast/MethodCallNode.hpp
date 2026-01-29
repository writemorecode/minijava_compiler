#ifndef METHOD_CALL_NODE_HPP
#define METHOD_CALL_NODE_HPP

#include "ast/Node.h"

class MethodCallNode : public Node {
    Node *object;
    Node *id;
    Node *exprList;

  public:
    MethodCallNode(Node *object, Node *id, Node *exprList, int l)
        : Node("Method call", l, {object, id, exprList}), object{object},
          id{id}, exprList{exprList} {};
    std::string checkTypes(SymbolTable &st) const override;
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
