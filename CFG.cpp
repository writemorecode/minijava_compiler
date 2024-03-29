#include "CFG.hpp"
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

BBlock *CFG::addMethodBlock(const std::string &name) {
    auto *ptr = new BBlock(name);
    methodBlocks.push_back(ptr);
    return ptr;
}

void CFG::generateBytecode(BytecodeProgram &program, SymbolTable &st) {
    for (auto *basicBlock : methodBlocks) {
        const auto &name = basicBlock->getName();
        auto &method = program.addBytecodeMethod(name);

        if (!name.ends_with("main")) {
            const auto *methodEntry = st.getMethodFromQualifiedName(name);
            if (methodEntry == nullptr) {
                std::cerr << "Qualified name '" << name
                          << "' not found in ST!\n";
                return;
            }
            const auto &params = methodEntry->getParameters();
            auto &bytecodeBlock = method.addBytecodeMethodBlock(name);
            for (auto it = params.rbegin(); it != params.rend(); ++it) {
                const auto &paramID = (*it)->getID();
                bytecodeBlock.store(paramID);
            }
        }
        basicBlock->generateBytecode(method, st);
    }
    const auto &mainName = methodBlocks.front()->getName();
    auto &mainBlock = program.getBytecodeMethod(mainName).getFirstBlock();
    mainBlock.stop();
}
