#include "CFG.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>

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

BBlock *CFG::addMethodRootBlock(const std::string &className,
                                const std::string &methodName) {
    auto *ptr = new BBlock(className, methodName);
    methodBlocks.push_back(ptr);
    return ptr;
}

void CFG::generateBytecode(BytecodeProgram &program, SymbolTable &st) {
    for (auto *basicBlock : methodBlocks) {
        const auto &name = basicBlock->getName();
        const auto *methodEntry = st.getMethodFromQualifiedName(name);
        const auto params = methodEntry->getParameterNames();

        auto &method = program.addBytecodeMethod(name);
        auto &bytecodeBlock = method.addBytecodeMethodBlock(name);
        std::for_each(params.rbegin(), params.rend(),
                      [&bytecodeBlock](const auto &param) {
                          bytecodeBlock.store(param);
                      });
        basicBlock->generateBytecode(method, st);
    }
    const auto &mainName = methodBlocks.front()->getName();
    auto &mainMethod = program.getBytecodeMethod(mainName);
    auto &mainBlock = mainMethod.getBytecodeMethodBlock(mainName);
    mainBlock.stop();
}
