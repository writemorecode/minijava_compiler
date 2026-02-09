#include "ast/MainClassNode.hpp"

#include "ast/AstVisitor.hpp"

void MainClassNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }

Operand MainClassNode::generateIR(CFG &graph, SymbolTable &st) {
    st.enterClassScope(mainClassName);
    st.enterMethodScope("main");
    graph.setCurrentBlock(graph.addMethodRootBlock(mainClassName, "main"));
    body->generateIR(graph, st);
    st.exitScope();
    st.exitScope();
    return graph.getCurrentBlock()->getName();
}
