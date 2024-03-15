#include "MethodNode.hpp"

bool MethodNode::buildTable(SymbolTable &st) const {
    bool valid = true;
    if (st.lookupMethod(id->value)) {
        std::cerr << "Error: (line " << lineno << ") Method '" << id->value
                  << "' already declared.\n";
        valid = false;
    }
    st.addMethod(type->value, id->value);
    auto currentMethod = st.lookupMethod(id->value);
    auto currentClass = dynamic_cast<Class *>(st.getCurrentRecord());
    currentClass->addMethod(currentMethod);

    st.enterScope("Method: " + id->value, currentMethod);
    bool validParams = params->buildTable(st);
    bool validBody = body->buildTable(st);
    st.exitScope();

    return valid && validParams && validBody;
}

std::string MethodNode::checkTypes(SymbolTable &st) const {
    st.enterScope("Method: " + id->value);
    const auto signatureReturnType = type->checkTypes(st);
    const auto bodyReturnType = body->checkTypes(st);
    st.exitScope();

    if (signatureReturnType != bodyReturnType) {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Return type '" << signatureReturnType << "' ";
        std::cerr << "in method '" << id->value << "' ";
        std::cerr << "does not match returned type '";
        std::cerr << bodyReturnType << "'.\n";
        return "";
    }

    return type->value;
}

std::string MethodNode::generateIR(CFG &graph) {
    graph.setCurrentBlock(graph.addMethodBlock());
    body->generateIR(graph);
    return graph.getCurrentBlock()->getName();
}
