#include "MethodParameterNode.hpp"

bool MethodParameterNode::buildTable(SymbolTable& st) const
{
    if (st.lookupVariableInScope(id->value)) {
        std::cerr << "Error: (line " << lineno << ") Parameter '" << id->value
                  << "' already declared.\n";
        return false;
    }
    st.addVariable(type->value, id->value);
    const auto parameter = st.lookupVariable(id->value);
    const auto currentScope = st.getCurrentScope();
    const auto currentMethod = dynamic_cast<Method*>(currentScope->getRecord());
    currentMethod->addParameter(parameter);
    return true;
}
