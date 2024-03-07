#include "ClassNode.hpp"

bool ClassNode::buildTable(SymbolTable& st) const
{
    if (st.lookupClass(id->value)) {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Class " << id->value << " already declared.\n";
    }

    st.addClass(id->value);
    auto currentClass = st.lookupClass(id->value);
    st.enterScope("Class: " + id->value, currentClass);
    st.addVariable(id->value, "this");
    body->buildTable(st);
    st.exitScope();

    return true;
}

std::string ClassNode::checkTypes(SymbolTable& st) const
{
    st.enterScope("Class: " + id->value);
    body->checkTypes(st);
    st.exitScope();
    return "";
}
