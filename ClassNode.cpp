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
    st.enterClassScope(currentClass);
    st.addVariable(id->value, "this");
    body->buildTable(st);
    st.exitScope();

    return valid;
}

std::string ClassNode::checkTypes(SymbolTable &st) const {
    st.enterClassScope(id->value);
    auto type = body->checkTypes(st);
    st.exitScope();
    if (type.empty()) {
        return "";
    }
    return "void";
}

Operand ClassNode::generateIR(CFG &graph, SymbolTable &st) {
    st.enterClassScope(id->value);
    body->generateIR(graph, st);
    st.exitScope();
    return "";
}
