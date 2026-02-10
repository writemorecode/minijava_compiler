#include "ast/MethodCallNode.hpp"

#include <cassert>

#include "ir/Tac.hpp"

Operand MethodCallNode::generateIR(CFG &graph, SymbolTable &st) {
    const auto *caller_type = graph.typeOf(*object);
    assert(caller_type != nullptr &&
           "Type information missing for method call receiver");
    if (caller_type == nullptr || caller_type->empty()) {
        return "";
    }

    auto *callingClass = st.lookupClass(*caller_type);
    if (callingClass == nullptr) {
        return "";
    }

    auto const *method = callingClass->lookupMethod(id->value);
    if (method == nullptr) {
        return "";
    }

    const auto &methodType = method->getType();
    object->generateIR(graph, st);

    for (const auto &arg : exprList->children) {
        const auto &argName = arg->generateIR(graph, st);
        graph.addInstruction(new ParamTac(argName));
    }

    const auto &name = graph.getTemporaryName();
    st.addVariable(methodType, name);

    const auto &methodName = id->value;
    const auto argCount = std::to_string(exprList->children.size());
    graph.addInstruction(
        new MethodCallTac(name, methodName, *caller_type, argCount));
    return name;
}
