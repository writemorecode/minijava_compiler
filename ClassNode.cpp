#include "ClassNode.hpp"

bool ClassNode::buildTable(SymbolTable &st) const {
    bool valid = true;
    if (st.lookupClass(id->value)) {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Class " << id->value << " already declared.\n";
        valid = false;
    }

    st.addClass(id->value);
    auto currentClass = st.lookupClass(id->value);
    st.enterScope("Class: " + id->value, currentClass);
    st.addVariable(id->value, "this");
    body->buildTable(st);
    st.exitScope();

    return valid;
}

std::string ClassNode::checkTypes(SymbolTable &st) const {
    st.enterScope("Class: " + id->value);
    auto type = body->checkTypes(st);
    st.exitScope();
    if (type.empty()) {
        return "";
    }
    return "void";
}

std::string ClassNode::generateIR(CFG &graph, SymbolTable &st) {
    st.enterScope("Class: " + id->value);
    body->generateIR(graph, st);
    st.exitScope();
    return "";
}
