#include "MainClassNode.hpp"

bool MainClassNode::buildTable(SymbolTable &st) const {
    if (st.lookupClass(mainClassName)) {
        std::cerr << "Error: (line " << lineno << ") Class '" << mainClassName
                  << "' already declared.\n";
        return false;
    }
    st.addClass(mainClassName);
    st.enterClassScope(mainClassName);
    st.addVariable(mainClassName, "this");
    st.addMethod("void", "main");
    st.enterMethodScope("main");
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
    const auto methodBlockName = mainClassName + ".main";
    graph.setCurrentBlock(graph.addMethodBlock(methodBlockName));
    body->generateIR(graph, st);
    st.exitScope();
    return graph.getCurrentBlock()->getName();
}
