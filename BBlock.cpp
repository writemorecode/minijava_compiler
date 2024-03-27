#include "BBlock.hpp"
#include "BytecodeInstruction.hpp"
#include "SymbolTable.hpp"
#include <algorithm>
#include <iomanip>
#include <iostream>

void BBlock::printBlockGraphviz(std::ostream &os) {
    markVisited();
    os << std::quoted(name) << " [label=\"";
    os << "[" << name << "]\n";
    for (const auto &instr : instructions) {
        instr->print(os);
    }
    os << "\"]\n";
    if (trueExit != nullptr) {
        os << std::quoted(name) << " -> " << trueExit->getName() << " ";
        os << "[xlabel=\"true\"];\n";
        if (!trueExit->isVisited()) {
            trueExit->printBlockGraphviz(os);
        }
    }
    if (falseExit != nullptr) {
        os << std::quoted(name) << " -> " << falseExit->getName() << " ";
        os << "[xlabel=\"false\"];\n";
        if (!falseExit->isVisited()) {
            falseExit->printBlockGraphviz(os);
        }
    }
}

void BBlock::addInstruction(Tac *ptr) { instructions.emplace_back(ptr); }

void BBlock::generateBytecode(BytecodeMethod &method, SymbolTable &st) {
    markGenerated();

    auto &methodBlock = method.getBytecodeMethodBlock(name);

    for (const auto &instruction : instructions) {
        instruction->generateBytecode(methodBlock);
    }

    if (hasTrueBlock() && !hasFalseBlock()) {
        methodBlock.jump(trueExit->name);
        if (!trueExit->isGenerated()) {
            trueExit->generateBytecode(method, st);
        }
    } else if (hasTrueBlock() && hasFalseBlock()) {
        methodBlock.cjump(falseExit->name);
        if (!trueExit->isGenerated()) {
            trueExit->generateBytecode(method, st);
        }
        if (!falseExit->isGenerated()) {
            falseExit->generateBytecode(method, st);
        }
    }
}
