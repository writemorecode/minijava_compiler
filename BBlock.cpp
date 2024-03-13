#include "BBlock.hpp"
#include <iostream>

void BBlock::printBlockGraphviz() {
    markVisited();
    std::cout << name << " [label=\"";
    std::cout << "[" << name << "]\n";
    for (const auto &instr : instructions) {
        instr->print();
    }
    std::cout << "\"]\n";
    if (trueExit != nullptr) {
        std::cout << name << " -> " << trueExit->getName() << " ";
        std::cout << "[xlabel=\"true\"];\n";
        if (!trueExit->isVisited()) {
            trueExit->printBlockGraphviz();
        }
    }
    if (falseExit != nullptr) {
        std::cout << name << " -> " << falseExit->getName() << " ";
        std::cout << "[xlabel=\"false\"];\n";
        if (!falseExit->isVisited()) {
            falseExit->printBlockGraphviz();
        }
    }
}

void BBlock::addInstruction(Tac *ptr) { instructions.emplace_back(ptr); }
