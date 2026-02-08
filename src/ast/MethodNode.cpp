#include "ast/MethodNode.hpp"

#include "ast/AstVisitor.hpp"

void MethodNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }

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
    auto *currentMethod = st.lookupMethod(methodName);
    st.enterMethodScope(currentMethod);
    graph.setCurrentBlock(
        graph.addMethodRootBlock(currentClass->getID(), methodName));
    body->generateIR(graph, st);
    st.exitScope();
    return graph.getCurrentBlock()->getName();
}
