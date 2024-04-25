#include "MethodCallWithoutArgumentsNode.hpp"

std::string MethodCallWithoutArgumentsNode::checkTypes(SymbolTable &st) const {
    const auto caller = object->checkTypes(st);
    if (caller == "") {
        return "";
    }
    const auto callingClass = st.lookupClass(caller);
    if (!callingClass) {
        std::cerr << "Error: (line " << lineno << ") Method '" << id->value
                  << "' not declared for class '" << caller << "'.\n";
        return "";
    }

    const auto method = callingClass->lookupMethod(id->value);
    if (method == nullptr) {
        std::cerr << "Error: (line " << lineno << ") Method '" << id->value
                  << "' not declared for class '" << callingClass->getID()
                  << "'.\n";
        return "";
    }

    const auto numExpectedArguments = method->getParameterCount();
    if (numExpectedArguments != 0) {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Method '" << method->getID();
        std::cerr << "' expects " << numExpectedArguments << " arguments, ";
        std::cerr << "no arguments passed.\n";
        return "";
    }
    return method->getType();
}

Operand MethodCallWithoutArgumentsNode::generateIR(CFG &graph,
                                                   SymbolTable &st) {
    const auto &caller = object->checkTypes(st);
    auto *callingClass = st.lookupClass(caller);
    auto const *method = callingClass->lookupMethod(id->value);
    const auto &methodType = method->getType();

    const auto &name = graph.getTemporaryName();
    st.addVariable(methodType, name);
    const auto &methodName = id->value;
    graph.addInstruction(new MethodCallTac(name, methodName, caller, "1"));
    return name;
}
