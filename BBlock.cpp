#include "BBlock.hpp"
#include <iostream>

void BBlock::printBlockGraphviz(std::ostream &os) {
    markVisited();
    os << name << " [label=\"";
    os << "[" << name << "]\n";
    for (const auto &instr : instructions) {
        instr->print(os);
    }
    os << "\"]\n";
    if (trueExit != nullptr) {
        os << name << " -> " << trueExit->getName() << " ";
        os << "[xlabel=\"true\"];\n";
        if (!trueExit->isVisited()) {
            trueExit->printBlockGraphviz(os);
        }
    }
    if (falseExit != nullptr) {
        os << name << " -> " << falseExit->getName() << " ";
        os << "[xlabel=\"false\"];\n";
        if (!falseExit->isVisited()) {
            falseExit->printBlockGraphviz(os);
        }
    }
}

void BBlock::addInstruction(Tac *ptr) { instructions.emplace_back(ptr); }
