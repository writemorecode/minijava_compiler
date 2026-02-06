#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "ir/CFG.hpp"

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
        const auto &className = basicBlock->getClassName();
        const auto &methodName = basicBlock->getMethodName();

        const auto *methodScope = st.resolveScope(className, methodName);
        const auto *method = dynamic_cast<Method *>(methodScope->getRecord());

        const auto methodParameters = method->getParameterNames();
        const auto &blockName = basicBlock->getName();
        const auto variableNames = methodScope->getVariableNames();
        std::vector<std::string> variables(variableNames.begin(),
                                            variableNames.end());
        auto &bytecodeMethod =
            program.addBytecodeMethod(blockName, std::move(variables));
        auto &bytecodeBlock = bytecodeMethod.addBytecodeMethodBlock(blockName);

        std::for_each(methodParameters.rbegin(), methodParameters.rend(),
                      [&bytecodeBlock](const auto &param) {
                          bytecodeBlock.store(param);
                      });

        basicBlock->generateBytecode(bytecodeMethod);
    }
    const auto mainName = methodBlocks.front()->getName();
    auto &mainMethod = program.getBytecodeMethod(mainName);
    auto &mainBlock = mainMethod.getBytecodeMethodBlock(mainName);
    mainBlock.stop();
}
