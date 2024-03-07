#include "StatementNode.hpp"

std::string AssignNode::checkTypes(SymbolTable& st) const
{
    const auto lhsType = id->checkTypes(st);
    const auto rhsType = expr->checkTypes(st);
    if (lhsType == rhsType) {
        return lhsType;
    }
    if (lhsType != "" && rhsType != "") {
        std::cerr << "Error: (line " << lineno << ") Cannot assign type '"
                  << rhsType << "' to type '" << lhsType << "'.\n";
    }
    return "";
}

bool AssignNode::buildTable(SymbolTable& st) const
{
    /*
     * This must be done before symbol table has been fully built.
     * If this is done after, during semantic analysis, the identifier
     * will be found, even though it was declared after this
     * statement.
     * */
    auto lookup = st.lookupVariable(id->value);
    if (!lookup) {
        std::cerr << "Error: (line " << lineno << ") ";
        std::cerr << "Identifier '" << id->value << "' ";
        std::cerr << "used before declaration in this scope.\n";
    }
    return true;
}

std::string ArrayAssignNode::checkTypes(SymbolTable& st) const
{
    const auto indexType = indexExpr->checkTypes(st);

    if (indexType != "int") {
        std::cerr << "Error: (line " << lineno << ") ";
        std::cerr << "Invalid array index type ";
        std::cerr << "'" << indexType << "', ";
        std::cerr << "expected type 'int'.\n";
        return "";
    }

    const auto lhsType = id->checkTypes(st);
    if (lhsType != "int[]") {
        std::cerr << "Error: (line " << lineno
                  << ") Invalid array type "
                     "'"
                  << lhsType << "', expected type 'int[]'.\n";
        return "";
    }

    const auto rhsType = rightExpr->checkTypes(st);
    if (rhsType != "int") {
        std::cerr << "Error: (line " << lineno
                  << ") Cannot assign value of type "
                     "'"
                  << rhsType << "', to array of type '" << lhsType << "'.\n";
        return "";
    }

    return "";
}
