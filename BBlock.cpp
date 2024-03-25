#include "BBlock.hpp"
#include "BytecodeInstruction.hpp"
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

void BBlock::generateBytecode(BytecodeMethod &method) {
    auto &methodBlock = method.addMethodBlock(name);
    for (const auto &instruction : instructions) {
        instruction->generateBytecode(methodBlock);
    }

    if (hasTrueBlock()) {
        if (hasFalseBlock()) {
            // iffalse _tcond goto false_block
            methodBlock.addBytecodeInstruction(
                new StringParameterInstruction(Opcode::CJMP, falseExit->name));
        } else {
            // goto true_block
            methodBlock.addBytecodeInstruction(
                new StringParameterInstruction(Opcode::JMP, trueExit->name));
        }
    }
}
