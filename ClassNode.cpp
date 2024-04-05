#include "ClassNode.hpp"

bool ClassNode::buildTable(SymbolTable &st) const {
    bool valid = true;
    if (st.lookupClass(className)) {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Class " << className << " already declared.\n";
        valid = false;
    }

    st.addClass(className);
    auto *currentClass = st.lookupClass(className);
    st.enterClassScope(currentClass);
    st.addVariable(className, "this");
    body->buildTable(st);
    st.exitScope();

    return valid;
}

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
