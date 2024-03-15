#include "CFG.hpp"
#include <iostream>

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

    if (rootBlock != nullptr) {
        rootBlock->printBlockGraphviz();
    }
    for (const auto &el : methods) {
        el->printBlockGraphviz();
    }

    std::cout << "}\n";
}

BBlock *CFG::newBlock() { return new BBlock(getBlockName()); }

void CFG::addInstruction(Tac *ptr) { currentBlock->addInstruction(ptr); }

BBlock *CFG::addMethodBlock() {
    auto *ptr = new BBlock(getBlockName());
    methods.push_back(ptr);
    return ptr;
}
