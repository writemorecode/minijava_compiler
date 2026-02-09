#include "ast/ClassNode.hpp"

#include "ast/AstVisitor.hpp"

void ClassNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }

Operand ClassNode::generateIR(CFG &graph, SymbolTable &st) {
    auto *currentClass = st.lookupClass(className);
    st.enterClassScope(currentClass);
    body->generateIR(graph, st);
    st.exitScope();
    return "";
}
