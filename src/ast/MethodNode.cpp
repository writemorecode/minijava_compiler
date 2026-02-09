#include "ast/MethodNode.hpp"

#include "ast/AstVisitor.hpp"

void MethodNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }

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
