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

std::string MethodCallWithoutArgumentsNode::generateIR(CFG &graph) {
    const auto &methodName = id->value;
    const auto &name = graph.getTemporaryName();
    const auto &callerName = object->generateIR(graph);
    graph.addInstruction(new ParamTac(callerName));
    graph.addInstruction(new MethodCallTac(name, methodName, "1"));
    return name;
}
