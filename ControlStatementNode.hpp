#ifndef CONTROL_STATEMENT_NODE_H
#define CONTROL_STATEMENT_NODE_H

#include "Node.h"

class ControlStatementNode : public Node {
    Node *cond, *stmts;

  public:
    ControlStatementNode(const std::string &t, Node *cond, Node *stmts, int l)
        : Node(t, l, {cond, stmts}), cond{cond}, stmts{stmts} {}
    std::string checkTypes(SymbolTable &st) const override;
};
class IfNode : public ControlStatementNode {
    Node *cond, *stmt;

  public:
    IfNode(Node *cond, Node *stmt, int l)
        : ControlStatementNode("If", cond, stmt, l), cond{cond}, stmt{stmt} {}
    std::string generateIR(CFG &graph) override;
};
class IfElseNode : public ControlStatementNode {
    Node *cond, *stmt, *elseStmt;

  public:
    IfElseNode(Node *cond, Node *stmt, Node *elseStmt, int l)
        : ControlStatementNode("If-else", cond, stmt, l), cond{cond},
          stmt{stmt}, elseStmt{elseStmt} {
        children.push_back(elseStmt);
    }
    std::string generateIR(CFG &graph) override;
};
class WhileNode : public ControlStatementNode {
    Node *cond, *stmt;

  public:
    WhileNode(Node *cond, Node *stmt, int l)
        : ControlStatementNode("While", cond, stmt, l), cond{cond}, stmt{stmt} {
    }
    std::string generateIR(CFG &graph) override;
};

#endif
