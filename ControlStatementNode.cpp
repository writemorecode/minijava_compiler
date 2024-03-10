
#include "ControlStatementNode.hpp"

std::string ControlStatementNode::checkTypes(SymbolTable &st) const {
    const auto condType = cond->checkTypes(st);
    if (!condType.empty() && condType != "boolean") {
        std::cerr << "Error: ";
        std::cerr << "(line " << lineno << ") ";
        std::cerr << "Condition for " << type << "-statement of invalid type "
                  << condType << ".\n";
        return "";
    }
    return "void";
}
