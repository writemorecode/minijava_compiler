#include "ArrayAccessNode.hpp"
#include "Tac.hpp"

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
