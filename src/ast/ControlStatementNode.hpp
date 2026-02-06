#ifndef CONTROL_STATEMENT_NODE_H
#define CONTROL_STATEMENT_NODE_H

#include "ast/Node.h"

class ControlStatementNode : public Node {
    Node *cond, *stmts;

  public:
    ControlStatementNode(const std::string &t, std::unique_ptr<Node> cond_,
                         std::unique_ptr<Node> stmts_, int l)
        : Node(t, l) {
        cond = append_child(std::move(cond_));
        stmts = append_child(std::move(stmts_));
    }
    std::string checkTypes(SymbolTable &st) const override;
};
class IfNode : public ControlStatementNode {
    Node *cond, *stmt;

  public:
    IfNode(std::unique_ptr<Node> cond_, std::unique_ptr<Node> stmt_, int l)
        : ControlStatementNode("If", std::move(cond_), std::move(stmt_), l) {
        auto it = children.begin();
        cond = it->get();
        ++it;
        stmt = it->get();
    }
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};
class IfElseNode : public ControlStatementNode {
    Node *cond, *stmt, *elseStmt;

  public:
    IfElseNode(std::unique_ptr<Node> cond_, std::unique_ptr<Node> stmt_,
               std::unique_ptr<Node> elseStmt_, int l)
        : ControlStatementNode("If-else", std::move(cond_), std::move(stmt_),
                               l) {
        auto it = children.begin();
        cond = it->get();
        ++it;
        stmt = it->get();
        elseStmt = append_child(std::move(elseStmt_));
    }
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};
class WhileNode : public ControlStatementNode {
    Node *cond, *stmt;

  public:
    WhileNode(std::unique_ptr<Node> cond_, std::unique_ptr<Node> stmt_, int l)
        : ControlStatementNode("While", std::move(cond_), std::move(stmt_), l) {
        auto it = children.begin();
        cond = it->get();
        ++it;
        stmt = it->get();
    }
    Operand generateIR(CFG &graph, SymbolTable &st) override;
};

#endif
