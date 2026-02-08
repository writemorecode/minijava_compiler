#include "ast/MethodWithoutParametersNode.hpp"

#include "ast/AstVisitor.hpp"

void MethodWithoutParametersNode::accept(AstVisitor &visitor) const {
    visitor.visit(*this);
}

bool MethodWithoutParametersNode::buildTable(SymbolTable &st) const {
    if (st.lookupMethod(id->value)) {
        std::cerr << "Error: (line " << lineno << ") Method '" << id->value
                  << "' already declared.\n";
        return false;
    }
    st.addMethod(type->value, id->value);
    auto *currentMethod = st.lookupMethod(id->value);
    auto *currentClass = dynamic_cast<Class *>(st.getCurrentRecord());
    currentClass->addMethod(currentMethod);

    st.enterMethodScope(currentMethod);
    bool validBody = body->buildTable(st);
    st.exitScope();

    return validBody;
}

std::string MethodWithoutParametersNode::checkTypes(SymbolTable &st) const {
    st.enterMethodScope(id->value);
    const auto signatureReturnType = type->checkTypes(st);
    const auto bodyReturnType = body->checkTypes(st);
    st.exitScope();

    if (bodyReturnType.empty()) {
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

Operand MethodWithoutParametersNode::generateIR(CFG &graph, SymbolTable &st) {
    auto *currentClass = dynamic_cast<Class *>(st.getCurrentRecord());
    st.enterMethodScope(id->value);
    graph.setCurrentBlock(
        graph.addMethodRootBlock(currentClass->getID(), id->value));
    body->generateIR(graph, st);
    st.exitScope();
    return graph.getCurrentBlock()->getName();
}
