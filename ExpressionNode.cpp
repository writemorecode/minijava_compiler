#include "ExpressionNode.hpp"
#include "Tac.hpp"

std::string ThisNode::checkTypes(SymbolTable &st) const {
    auto const *lookup = st.lookupVariable(type);
    if (!lookup) {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Undeclared identifier (type " << type << ")" << value
                  << ".\n";
        return "";
    }
    return lookup->getType();
}
std::string ThisNode::generateIR(CFG &graph, SymbolTable &st) { return value; }
std::string BooleanNode::checkTypes(SymbolTable &st) const { return "boolean"; }
std::string BooleanNode::generateIR(CFG &graph, SymbolTable &st) {
    return value;
}
std::string ClassAllocationNode::checkTypes(SymbolTable &st) const {
    return id;
}
std::string ClassAllocationNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addVariable(id, name);
    graph.addInstruction(new NewTac(name, id));
    return name;
}
std::string NotNode::checkTypes(SymbolTable &st) const {
    const auto exprType = expr->checkTypes(st);
    if (exprType == "boolean") {
        return "boolean";
    }
    std::cerr << "Error: ";
    std::cerr << "(line " << lineno << ") ";
    std::cerr << "Invalid type '" << exprType
              << "' for negation operator, expected type 'boolean'.\n";
    return "";
}

std::string NotNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addBooleanVariable(name);
    auto rhsName = expr->generateIR(graph, st);
    graph.addInstruction(new NotTac(name, rhsName));
    return name;
}

std::string ArrayAccessNode::checkTypes(SymbolTable &st) const {
    const auto arrayType = array->checkTypes(st);
    const auto indexType = index->checkTypes(st);

    if (indexType != "int") {
        std::cerr << "Error: (line " << lineno << ") ";
        std::cerr << "Invalid array index type ";
        std::cerr << "'" << indexType << "', ";
        std::cerr << "expected type 'int'.\n";
        return "";
    }
    if (arrayType != "int[]") {
        std::cerr << "Error: (line " << lineno << ") ";
        std::cerr << "Invalid array type ";
        std::cerr << "'" << indexType << "', ";
        std::cerr << "expected type 'int[]'.\n";
        return "";
    }

    return "int";
}

std::string ArrayAccessNode::generateIR(CFG &graph, SymbolTable &st) {
    auto indexName = index->generateIR(graph, st);
    auto arrayName = array->value;
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    graph.addInstruction(new ArrayAccessTac(name, arrayName, indexName));
    return name;
}

std::string IntegerArrayAllocationNode::checkTypes(SymbolTable &st) const {
    const auto lengthType = length->checkTypes(st);
    if (lengthType != "int") {
        std::cerr << "Error: (line " << lineno << ") Invalid type '"
                  << lengthType
                  << "' for array length"
                     ", expected type 'int'.\n";
        return "";
    }
    return "int[]";
}
std::string IntegerArrayAllocationNode::generateIR(CFG &graph,
                                                   SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addVariable("int[]", name);
    auto lengthName = length->generateIR(graph, st);
    graph.addInstruction(new NewArrayTac(name, lengthName));
    return name;
}
std::string ArrayLengthNode::checkTypes(SymbolTable &st) const {
    const auto arrayType = array->checkTypes(st);
    if (arrayType != "int[]") {
        std::cerr << "Error: (line " << lineno << ") Invalid type '"
                  << arrayType
                  << "' for array length, "
                     "expected type 'int[]'.\n";
        return "";
    }
    return "int";
}

std::string ArrayLengthNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    auto arrayName = array->value;
    graph.addInstruction(new UnaryExpressionTac(name, "length", arrayName));
    return name;
}
