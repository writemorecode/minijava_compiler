
#include "MethodCallNode.hpp"
#include "Tac.hpp"

std::string MethodCallNode::checkTypes(SymbolTable &st) const {
    const auto caller = object->checkTypes(st);
    if (caller.empty()) {
        return "";
    }
    auto *callingClass = st.lookupClass(caller);
    if (!callingClass) {
        std::cerr << "Error: (line " << lineno << ") Method '" << id->value
                  << "' not declared for class '" << caller << "'.\n";
        return "";
    }

    auto *const method = callingClass->lookupMethod(id->value);
    if (method == nullptr) {
        std::cerr << "Error: (line " << lineno << ") Method '" << id->value
                  << "' not declared for class '" << callingClass->getID()
                  << "'.\n";
        return "";
    }

    const auto numExpectedArguments = method->getParameterCount();
    const auto numPassedArguments = exprList->children.size();

    if (numPassedArguments != numExpectedArguments) {
        std::cerr << "Error: (line " << lineno << ") Method '"
                  << method->getID() << "' expects " << numExpectedArguments
                  << " arguments, " << numPassedArguments
                  << " arguments given.\n";
        return "";
    }

    const auto &params = method->getParameters();
    const auto &args = exprList->children;
    auto paramsIter = params.begin();
    auto argsIter = args.begin();

    while (paramsIter != params.end() && argsIter != args.end()) {
        const auto paramType = (*paramsIter)->getType();
        const auto argType = (*argsIter)->checkTypes(st);
        if (argType.empty()) {
            return "";
        }
        const auto argNumber = 1 + std::distance(args.cbegin(), argsIter);
        if (paramType != argType) {
            std::cerr << "Error: (line " << lineno << ") Argument " << argNumber
                      << " of type '" << argType
                      << "' does not match parameter " << argNumber
                      << " of type '" << paramType << "'.\n";
            return "";
        }
        ++paramsIter;
        ++argsIter;
    }
    return method->getType();
}

std::string MethodCallNode::generateIR(CFG &graph) {
    const auto &methodName = id->value;
    const auto argCount = std::to_string(exprList->children.size() + 1);
    const auto &name = graph.getTemporaryName();

    const auto &callerName = object->generateIR(graph);
    graph.addInstruction(new ParamTac(callerName));
    for (const auto &arg : exprList->children) {
        const auto &argName = arg->generateIR(graph);
        graph.addInstruction(new ParamTac(argName));
    }

    graph.addInstruction(new MethodCallTac(name, methodName, argCount));
    return name;
}
