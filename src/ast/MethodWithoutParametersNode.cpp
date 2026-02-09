#include "ast/MethodWithoutParametersNode.hpp"

#include "ast/AstVisitor.hpp"

void MethodWithoutParametersNode::accept(AstVisitor &visitor) const {
    visitor.visit(*this);
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
