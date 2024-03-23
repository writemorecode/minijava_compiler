#include "IntegerArrayAllocationNode.hpp"
#include "Tac.hpp"

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

Operand IntegerArrayAllocationNode::generateIR(CFG &graph,
                                                   SymbolTable &st) {
    auto name = graph.getTemporaryName();
    st.addVariable("int[]", name);
    auto lengthName = length->generateIR(graph, st);
    graph.addInstruction(new NewArrayTac(name, lengthName));
    return name;
}
