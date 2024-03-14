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

std::string IfElseNode::generateIR(CFG &graph) {
    auto *trueBlock = graph.newBlock();
    auto *falseBlock = graph.newBlock();
    auto *joinBlock = graph.newBlock();

    const auto &falseLabel = falseBlock->getName();
    const auto &joinLabel = joinBlock->getName();

    const auto &condName = cond->generateIR(graph);
    graph.addInstruction(new CondJumpTac(falseLabel, condName));

    auto *currentBlock = graph.getCurrentBlock();
    currentBlock->setTrueBlock(trueBlock);
    currentBlock->setFalseBlock(falseBlock);

    trueBlock->setTrueBlock(joinBlock);
    falseBlock->setTrueBlock(joinBlock);

    graph.setCurrentBlock(trueBlock);
    stmt->generateIR(graph);
    graph.addInstruction(new JumpTac(joinLabel));

    graph.setCurrentBlock(falseBlock);
    elseStmt->generateIR(graph);
    graph.addInstruction(new JumpTac(joinLabel));

    graph.setCurrentBlock(joinBlock);

    return "placeholder ifelsenode::generateir";
}

std::string WhileNode::generateIR(CFG &graph) {
    auto *headerBlock = graph.newBlock();
    const auto &headerLabel = headerBlock->getName();
    auto *bodyBlock = graph.newBlock();
    bodyBlock->setTrueBlock(headerBlock);
    auto *joinBlock = graph.newBlock();

    auto *currentBlock = graph.getCurrentBlock();
    currentBlock->setTrueBlock(headerBlock);
    graph.addInstruction(new JumpTac(headerLabel));

    graph.setCurrentBlock(headerBlock);
    const auto &condName = cond->generateIR(graph);
    graph.addInstruction(new CondJumpTac(joinBlock->getName(), condName));

    graph.setCurrentBlock(bodyBlock);
    currentBlock->setTrueBlock(headerBlock);
    stmt->generateIR(graph);
    graph.getCurrentBlock()->setTrueBlock(headerBlock);
    graph.addInstruction(new JumpTac(headerLabel));

    headerBlock->setTrueBlock(bodyBlock);
    headerBlock->setFalseBlock(joinBlock);

    graph.setCurrentBlock(joinBlock);

    return "placeholder whilenode::generateir";
}
