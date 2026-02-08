#include "ast/ClassNode.hpp"

#include "ast/AstVisitor.hpp"

void ClassNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }

std::string ClassNode::checkTypes(SymbolTable &st) const {
    st.enterClassScope(className);
    auto type = body->checkTypes(st);
    st.exitScope();
    if (type.empty()) {
        return "";
    }
    return "void";
}

Operand ClassNode::generateIR(CFG &graph, SymbolTable &st) {
    auto *currentClass = st.lookupClass(className);
    st.enterClassScope(currentClass);
    body->generateIR(graph, st);
    st.exitScope();
    return "";
}
