#include <algorithm>
#include <iostream>
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

const std::string *CFG::typeOf(const Node &node) const {
    if (type_info_ == nullptr) {
        return nullptr;
    }
    return type_info_->get(node);
}

void CFG::generateBytecode(BytecodeProgram &program, SymbolTable &st) {
    BBlock *mainRoot = nullptr;

    for (auto *basicBlock : methodBlocks) {
        if (mainRoot == nullptr) {
            mainRoot = basicBlock;
        }

        const auto &className = basicBlock->getClassName();
        const auto &methodName = basicBlock->getMethodName();

        auto *methodScope = st.resolveScope(className, methodName);
        const auto *method = dynamic_cast<Method *>(methodScope->getRecord());

        const auto methodParameters = method->getParameterNames();
        const auto &blockName = basicBlock->getName();
        auto variableNames = methodScope->getVariableNames();
        if (const auto *classScope = methodScope->getParent();
            classScope != nullptr) {
            const auto classVariableNames = classScope->getVariableNames();
            variableNames.insert(classVariableNames.begin(),
                                 classVariableNames.end());
        }

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

    if (mainRoot != nullptr) {
        auto &mainMethod = program.getBytecodeMethod(mainRoot->getName());
        appendStopToLeafBlocks(mainRoot, mainMethod);
    }
}
