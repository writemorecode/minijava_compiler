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

void CFG::printGraphviz(std::ostream &os) const {
    os << "digraph {\n";
    os << "graph [splines=ortho]\n";
    os << "node [shape=box]\n";
    for (const auto &el : methods) {
        el->printBlockGraphviz(os);
    }
    os << "}\n";
}

BBlock *CFG::newBlock() { return new BBlock(getBlockName()); }

void CFG::addInstruction(Tac *ptr) { currentBlock->addInstruction(ptr); }

BBlock *CFG::addMethodBlock() {
    auto *ptr = new BBlock(getBlockName());
    methods.push_back(ptr);
    return ptr;
}

BBlock *CFG::addMethodBlock(const std::string &name) {
    auto *ptr = new BBlock(name);
    methods.push_back(ptr);
    return ptr;
}

// void CFG::generateBytecode(BytecodeProgram &program) {
//     for (const auto *methodBlock : methods) {
//         auto &method = program.addMethod(methodBlock->getName());
//         methodBlock->generateBytecode(method);
//     }
// }
