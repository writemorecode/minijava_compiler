#include "ast/MethodWithoutParametersNode.hpp"

#include "ast/AstVisitor.hpp"

void MethodWithoutParametersNode::accept(AstVisitor &visitor) const {
    visitor.visit(*this);
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
        return "";
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
