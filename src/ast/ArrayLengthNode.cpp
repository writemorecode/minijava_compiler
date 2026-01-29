#include "ast/ArrayLengthNode.hpp"
#include "ir/Tac.hpp"

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

Operand ArrayLengthNode::generateIR(CFG &graph, SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addIntegerVariable(name);
    auto arrayName = array->value;
    graph.addInstruction(new ArrayLengthTac(name, arrayName));
    return name;
}
