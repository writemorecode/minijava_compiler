#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

#include "ir/CFG.hpp"
#include "semantic/TypeCheckVisitor.hpp"

namespace {
void appendStopToLeafBlocks(BBlock *root, BytecodeMethod &method) {
    std::vector<BBlock *> stack{root};
    std::unordered_set<BBlock *> visited;

    while (!stack.empty()) {
        auto *block = stack.back();
        stack.pop_back();

        if (!visited.insert(block).second) {
            continue;
        }

        const auto hasTrueBlock = block->hasTrueBlock();
        const auto hasFalseBlock = block->hasFalseBlock();
        if (!hasTrueBlock && !hasFalseBlock) {
            method.getBytecodeMethodBlock(block->getName()).stop();
            continue;
        }

        if (hasTrueBlock) {
            stack.push_back(block->getTrueBlock());
        }
        if (hasFalseBlock) {
            stack.push_back(block->getFalseBlock());
        }
    }
}
} // namespace

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
    resetVisitedFlags();
    os << "digraph {\n";
    os << "graph [splines=ortho]\n";
    os << "node [shape=box]\n";
    for (auto *el : methodRoots) {
        el->printBlockGraphviz(os);
    }
    for (const auto &block : allBlocks) {
        if (!block->isVisited()) {
            block->printBlockGraphviz(os);
        }
    }
    os << "}\n";
}

BBlock *CFG::ownBlock(std::unique_ptr<BBlock> block) {
    auto *ptr = block.get();
    allBlocks.push_back(std::move(block));
    return ptr;
}

void CFG::resetVisitedFlags() const {
    for (const auto &block : allBlocks) {
        block->resetVisited();
    }
}

void CFG::resetGeneratedFlags() const {
    for (const auto &block : allBlocks) {
        block->resetGenerated();
    }
}

BBlock *CFG::newBlock() {
    return ownBlock(std::make_unique<BBlock>(getBlockName()));
}

void CFG::addInstruction(Tac *ptr) { currentBlock->addInstruction(ptr); }

BBlock *CFG::addMethodBlock() {
    auto *ptr = ownBlock(std::make_unique<BBlock>(getBlockName()));
    methodRoots.push_back(ptr);
    return ptr;
}

BBlock *CFG::addMethodRootBlock(const std::string &className,
                                const std::string &methodName) {
    auto *ptr = ownBlock(std::make_unique<BBlock>(className, methodName));
    methodRoots.push_back(ptr);
    return ptr;
}

const std::string *CFG::typeOf(const Node &node) const {
    if (type_info_ == nullptr) {
        return nullptr;
    }
    return type_info_->get(node);
}

void CFG::generateBytecode(BytecodeProgram &program, SymbolTable &st) {
    resetGeneratedFlags();
    BBlock *mainRoot = nullptr;

    for (auto *basicBlock : methodRoots) {
        if (mainRoot == nullptr) {
            mainRoot = basicBlock;
        }

        const auto &className = basicBlock->getClassName();
        const auto &methodName = basicBlock->getMethodName();

        auto *methodScope = st.resolveScope(className, methodName);
        const auto *method = dynamic_cast<Method *>(methodScope->getRecord());
        const auto *classScope = methodScope->getParent();

        const auto methodParameters = method->getParameterNames();
        const auto &blockName = basicBlock->getName();
        const auto variableNames = methodScope->getVariableNames();
        auto fieldVariableNames = classScope != nullptr
                                      ? classScope->getVariableNames()
                                      : std::set<std::string>{};

        std::vector<std::string> variables(variableNames.begin(),
                                           variableNames.end());
        std::vector<std::string> fieldVariables(fieldVariableNames.begin(),
                                                fieldVariableNames.end());
        auto &bytecodeMethod = program.addBytecodeMethod(
            blockName, std::move(variables), std::move(fieldVariables));
        auto &bytecodeBlock = bytecodeMethod.addBytecodeMethodBlock(blockName);

        if (basicBlock != mainRoot) {
            bytecodeBlock.store("this");
        }

        std::for_each(methodParameters.rbegin(), methodParameters.rend(),
                      [&bytecodeBlock](const auto &param) {
                          bytecodeBlock.store(param);
                      });

        basicBlock->generateBytecode(bytecodeMethod);
    }

    if (mainRoot != nullptr) {
        auto &mainMethod = program.getBytecodeMethod(mainRoot->getName());
        appendStopToLeafBlocks(mainRoot, mainMethod);
    }
}
