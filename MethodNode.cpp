#include "MethodNode.hpp"

bool MethodNode::buildTable(SymbolTable &st) const {
    bool valid = true;
    if (st.lookupMethod(methodName)) {
        std::cerr << "Error: (line " << lineno << ") Method '" << methodName
                  << "' already declared.\n";
        valid = false;
    }

    st.addMethod(methodType, methodName);
    auto *currentMethod = st.lookupMethod(methodName);
    auto *currentClass = dynamic_cast<Class *>(st.getCurrentRecord());
    currentClass->addMethod(currentMethod);

    st.enterMethodScope(currentMethod);
    bool validParams = params->buildTable(st);
    bool validBody = body->buildTable(st);
    st.exitScope();

    return valid && validParams && validBody;
}

std::string MethodNode::checkTypes(SymbolTable &st) const {
    st.enterMethodScope(methodName);
    const auto signatureReturnType = type->checkTypes(st);
    const auto bodyReturnType = body->checkTypes(st);
    st.exitScope();

    if (signatureReturnType != bodyReturnType) {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Return type '" << signatureReturnType << "' ";
        std::cerr << "in method '" << methodName << "' ";
        std::cerr << "does not match returned type '";
        std::cerr << bodyReturnType << "'.\n";
        return "";
    }

    return methodType;
}

Operand MethodNode::generateIR(CFG &graph, SymbolTable &st) {
    auto *currentClass = dynamic_cast<Class *>(st.getCurrentRecord());
    st.enterMethodScope(methodName);
    const auto methodBlockName = currentClass->getID() + "." + methodName;
    graph.setCurrentBlock(graph.addMethodBlock(methodBlockName));
    body->generateIR(graph, st);
    st.exitScope();
    return graph.getCurrentBlock()->getName();
}
