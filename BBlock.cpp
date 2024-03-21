#include "BBlock.hpp"
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
