#include "CFG.hpp"
#include "BytecodeInstruction.hpp"
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
    for (const auto &el : methodBlocks) {
        el->printBlockGraphviz(os);
    }
    os << "}\n";
}

BBlock *CFG::newBlock() { return new BBlock(getBlockName()); }

void CFG::addInstruction(Tac *ptr) { currentBlock->addInstruction(ptr); }

BBlock *CFG::addMethodBlock() {
    auto *ptr = new BBlock(getBlockName());
    methodBlocks.push_back(ptr);
    return ptr;
}

BBlock *CFG::addMethodBlock(const std::string &name) {
    auto *ptr = new BBlock(name);
    methodBlocks.push_back(ptr);
    return ptr;
}

void CFG::generateBytecode(BytecodeProgram &program) {
    for (auto *basicBlock : methodBlocks) {
        const auto &name = basicBlock->getName();
        program.addMethod(name);
        auto &method = program.getMethod(name);
        basicBlock->generateBytecode(method);
    }
    const auto &mainName = methodBlocks.front()->getName();
    auto &mainBlock = program.getMethod(mainName).getFirstBlock();
    mainBlock.addBytecodeInstruction(
        new StackParameterInstruction(Opcode::STOP));
}
