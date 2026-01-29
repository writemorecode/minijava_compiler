#ifndef CONTROL_STATEMENT_NODE_H
#define CONTROL_STATEMENT_NODE_H

#include "ast/Node.h"

class ControlStatementNode : public Node {
    Node *cond, *stmts;

  public:
    ControlStatementNode(const std::string &t, Node *cond_, Node *stmts_, int l)
        : Node(t, l, {cond_, stmts_}), cond(cond_), stmts(stmts_) {}
    std::string checkTypes(SymbolTable &st) const override;
};
class IfNode : public ControlStatementNode {
    Node *cond, *stmt;

  public:
    IfNode(Node *cond, Node *stmt, int l)
        : ControlStatementNode("If", cond, stmt, l), cond{cond}, stmt{stmt} {}
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};
class IfElseNode : public ControlStatementNode {
    Node *cond, *stmt, *elseStmt;

  public:
    IfElseNode(Node *cond, Node *stmt, Node *elseStmt, int l)
        : ControlStatementNode("If-else", cond, stmt, l), cond{cond},
          stmt{stmt}, elseStmt{elseStmt} {
        children.push_back(elseStmt);
    }
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};
class WhileNode : public ControlStatementNode {
    Node *cond, *stmt;

  public:
    WhileNode(Node *cond, Node *stmt, int l)
        : ControlStatementNode("While", cond, stmt, l), cond{cond}, stmt{stmt} {
    }
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
