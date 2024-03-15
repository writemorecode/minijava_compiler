#include "MethodBodyNode.hpp"

std::string MethodBodyNode::checkTypes(SymbolTable &st) const {
    body->checkTypes(st);
    return returnValue->checkTypes(st);
}
std::string MethodBodyNode::generateIR(CFG &graph) {
    body->generateIR(graph);
    const auto &name = returnValue->generateIR(graph);
    graph.addInstruction(new ReturnTac(name));
    return name;
}

std::string ReturnOnlyMethodBodyNode::checkTypes(SymbolTable &st) const {
    return returnValue->checkTypes(st);
}
std::string ReturnOnlyMethodBodyNode::generateIR(CFG &graph) {
    const auto &name = returnValue->generateIR(graph);
    graph.addInstruction(new ReturnTac(name));
    return name;
}
