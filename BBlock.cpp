#include "BBlock.hpp"
#include <iostream>

void BBlock::printGraphviz() const {
    std::cout << name << " [label=\"";
    std::cout << "[" << name << "]\n";
    for (const auto &instr : instructions) {
        instr->print();
    }
    std::cout << "\"]\n";
    if (trueExit != nullptr && !trueExit->visited) {
        std::cout << name << " -> " << trueExit->name << " ";
        std::cout << "[xlabel=\"true\"];\n";
        trueExit->printGraphviz();
    }
    if (falseExit != nullptr && !falseExit->visited) {
        std::cout << name << " -> " << falseExit->name << " ";
        std::cout << "[xlabel=\"false\"];\n";
        falseExit->printGraphviz();
    }
}

void BBlock::addInstruction(Tac *ptr) { instructions.emplace_back(ptr); }
