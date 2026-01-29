#include "ast/MethodParameterNode.hpp"

bool MethodParameterNode::buildTable(SymbolTable &st) const {
    if (st.lookupVariableInScope(id->value)) {
        std::cerr << "Error: (line " << lineno << ") Parameter '" << id->value
                  << "' already declared.\n";
        return false;
    }
    st.addVariable(type->value, id->value);
    auto *parameter = st.lookupVariable(id->value);
    auto *currentScope = st.getCurrentScope();
    auto *currentMethod = dynamic_cast<Method *>(currentScope->getRecord());
    currentMethod->addParameter(parameter);
    return true;
}
