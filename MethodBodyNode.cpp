#include "MethodBodyNode.hpp"

std::string MethodBodyNode::checkTypes(SymbolTable &st) const {
    body->checkTypes(st);
    return returnValue->checkTypes(st);
}
std::string MethodBodyNode::generateIR(CFG &graph, SymbolTable &st) {
    body->generateIR(graph, st);
    const auto &name = returnValue->generateIR(graph, st);
    graph.addInstruction(new ReturnTac(name));
    return name;
}

std::string ReturnOnlyMethodBodyNode::checkTypes(SymbolTable &st) const {
    return returnValue->checkTypes(st);
}
std::string ReturnOnlyMethodBodyNode::generateIR(CFG &graph, SymbolTable &st) {
    const auto &name = returnValue->generateIR(graph, st);
    graph.addInstruction(new ReturnTac(name));
    return name;
}
