#include "ControlStatementNode.hpp"
#include "Tac.hpp"

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

std::string IfNode::generateIR(CFG &graph) {
    auto *trueBlock = graph.newBlock();
    auto *joinBlock = graph.newBlock();
    const auto &joinLabel = joinBlock->getName();

    const auto &condName = cond->generateIR(graph);
    const auto &trueLabel = trueBlock->getName();
    graph.addInstruction(new CondJumpTac(joinLabel, condName));

    auto *currentBlock = graph.getCurrentBlock();
    currentBlock->setTrueBlock(trueBlock);
    currentBlock->setFalseBlock(joinBlock);

    trueBlock->setTrueBlock(joinBlock);

    graph.setCurrentBlock(trueBlock);
    stmt->generateIR(graph);
    graph.addInstruction(new JumpTac(joinLabel));

    graph.setCurrentBlock(joinBlock);

    return "placeholder ifnode::generateir";
}
