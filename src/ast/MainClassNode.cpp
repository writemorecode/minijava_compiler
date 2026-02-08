#include "ast/MainClassNode.hpp"

#include "ast/AstVisitor.hpp"

void MainClassNode::accept(AstVisitor &visitor) const { visitor.visit(*this); }

bool MainClassNode::buildTable(SymbolTable &st) const {
    if (st.lookupClass(mainClassName)) {
        std::cerr << "Error: (line " << lineno << ") Class '" << mainClassName
                  << "' already declared.\n";
        return false;
    }
    st.addClass(mainClassName);
    auto *mainClass = st.lookupClass(mainClassName);
    st.enterClassScope(mainClass);

    st.addVariable(mainClassName, "this");
    Variable *mainClassThis = st.lookupVariableInScope("this");
    mainClass->addVariable(mainClassThis);

    st.addMethod("void", "main");
    Method *mainClassMethod = st.lookupMethod("main");
    mainClass->addMethod(mainClassMethod);

    st.enterMethodScope(mainClassMethod);
    st.addVariable("String[]", mainMethodArgumentName);
    st.exitScope();
    st.exitScope();
    return true;
}

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
