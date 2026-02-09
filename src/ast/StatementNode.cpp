#include "ast/StatementNode.hpp"
#include "ir/Tac.hpp"

Operand AssignNode::generateIR(CFG &graph, SymbolTable &st) {
    auto rhsName = expr->generateIR(graph, st);
    auto lhsName = id->value;
    graph.addInstruction(new CopyTac(rhsName, lhsName));
    return lhsName;
}

Operand ArrayAssignNode::generateIR(CFG &graph, SymbolTable &st) {
    auto indexName = indexExpr->generateIR(graph, st);
    auto rhsName = rightExpr->generateIR(graph, st);
    auto arrayName = id->value;
    graph.addInstruction(new ArrayCopyTac(arrayName, indexName, rhsName));
    return arrayName;
}

Operand PrintNode::generateIR(CFG &graph, SymbolTable &st) {
    const auto &value = expr->generateIR(graph, st);
    graph.addInstruction(new PrintTac(value));
    return "";
}
