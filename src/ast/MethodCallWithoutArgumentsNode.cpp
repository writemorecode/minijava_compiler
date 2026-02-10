#include "ast/MethodCallWithoutArgumentsNode.hpp"
#include <cassert>

Operand MethodCallWithoutArgumentsNode::generateIR(CFG &graph,
                                                   SymbolTable &st) {
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
    const auto receiver = object->generateIR(graph, st);

    const auto &name = graph.getTemporaryName();
    st.addVariable(methodType, name);
    const auto &methodName = id->value;
    const auto methodTarget = *caller_type + "." + methodName;
    graph.addInstruction(new MethodCallTac(name, receiver, methodTarget, 0));
    return name;
}
