#include "CFG.hpp"
#include "Node.h"

std::string CFG::getTemporaryName() {
    auto name = "_t" + std::to_string(temporaryIndex);
    temporaryIndex++;
    return name;
}

std::string CFG::getBlockName() {
    auto name = "block_" + std::to_string(blockIndex);
    blockIndex++;
    return name;
}

void CFG::printGraphviz() const {
    std::cout << "digraph {\n";
    std::cout << "graph [splines=ortho]\n";
    std::cout << "node [shape=box]\n";

    currentBlock->printGraphviz();

    std::cout << "}\n";
}

BBlock *CFG::newBlock() { return new BBlock(getBlockName()); }

void CFG::addInstruction(Tac *ptr) {
    if (!currentBlock) {
        currentBlock = new BBlock(getBlockName());
    }
    currentBlock->addInstruction(ptr);
}
