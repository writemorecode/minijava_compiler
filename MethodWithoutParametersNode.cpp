#include "MethodWithoutParametersNode.hpp"

bool MethodWithoutParametersNode::buildTable(SymbolTable &st) const {
    if (st.lookupMethod(id->value)) {
        std::cerr << "Error: (line " << lineno << ") Method '" << id->value
                  << "' already declared.\n";
        return false;
    }
    st.addMethod(type->value, id->value);
    auto currentMethod = st.lookupMethod(id->value);
    auto currentClass = dynamic_cast<Class *>(st.getCurrentRecord());
    currentClass->addMethod(currentMethod);

    st.enterScope("Method: " + id->value, currentMethod);
    body->buildTable(st);
    st.exitScope();

    return true;
}

std::string MethodWithoutParametersNode::checkTypes(SymbolTable &st) const {
    st.enterScope("Method: " + id->value);
    const auto signatureReturnType = type->checkTypes(st);
    const auto bodyReturnType = body->checkTypes(st);
    st.exitScope();

    if (bodyReturnType == "") {
        return "";
    }

    if (signatureReturnType != bodyReturnType) {
        std::cerr << "Error: (line " << lineno << ") Return type '"
                  << signatureReturnType << "' in method '" << id->value
                  << "' does not match returned type '" << bodyReturnType
                  << "'.\n";
    }

    return type->value;
}

std::string MethodWithoutParametersNode::generateIR(CFG &graph,
                                                    SymbolTable &st) {
    st.enterScope("Method: " + id->value);
    graph.setCurrentBlock(graph.addMethodBlock());
    body->generateIR(graph, st);
    st.exitScope();
    return graph.getCurrentBlock()->getName();
}
