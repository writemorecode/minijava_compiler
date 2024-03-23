#include "MainClassNode.hpp"

bool MainClassNode::buildTable(SymbolTable &st) const {
    if (st.lookupClass(id->value)) {
        std::cerr << "Error: (line " << lineno << ") Class '" << id->value
                  << "' already declared.\n";
        return false;
    }
    st.addClass(id->value);
    st.enterScope("Class: " + id->value);
    st.addVariable(id->value, "this");
    st.addMethod("void", "main");
    st.enterScope("Method: main");
    st.addVariable("String[]", arg->value);
    st.exitScope();
    st.exitScope();
    return true;
}

std::string MainClassNode::checkTypes(SymbolTable &st) const {
    st.enterScope("Class: " + id->value);
    body->checkTypes(st);
    st.exitScope();
    return "void";
}

Operand MainClassNode::generateIR(CFG &graph, SymbolTable &st) {
    st.enterScope("Class: " + id->value);
    const auto methodBlockName = id->value + ".main";
    graph.setCurrentBlock(graph.addMethodBlock(methodBlockName));
    body->generateIR(graph, st);
    st.exitScope();
    return graph.getCurrentBlock()->getName();
}
