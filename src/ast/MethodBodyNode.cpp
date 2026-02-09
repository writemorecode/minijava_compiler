#include "ast/MethodBodyNode.hpp"

Operand MethodBodyNode::generateIR(CFG &graph, SymbolTable &st) {
    body->generateIR(graph, st);
    const auto &name = returnValue->generateIR(graph, st);
    graph.addInstruction(new ReturnTac(name));
    return name;
}

Operand ReturnOnlyMethodBodyNode::generateIR(CFG &graph, SymbolTable &st) {
    const auto &name = returnValue->generateIR(graph, st);
    graph.addInstruction(new ReturnTac(name));
    return name;
}
