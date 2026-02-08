#include "ast/MainClassNode.hpp"

#include "ast/AstVisitor.hpp"

void MainClassNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }

std::string MainClassNode::checkTypes(SymbolTable &st) const {
    st.enterClassScope(mainClassName);
    body->checkTypes(st);
    st.exitScope();
    return "void";
}

Operand MainClassNode::generateIR(CFG &graph, SymbolTable &st) {
    st.enterClassScope(mainClassName);
    st.enterMethodScope("main");
    graph.setCurrentBlock(graph.addMethodRootBlock(mainClassName, "main"));
    body->generateIR(graph, st);
    st.exitScope();
    st.exitScope();
    return graph.getCurrentBlock()->getName();
}
