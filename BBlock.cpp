#include "BBlock.hpp"
#include <iostream>

void BBlock::printGraphviz(int &count) const {
    int id = count;
    std::cout << name << " [label=\"";
    std::cout << name << "\n";
    for (const auto &instr : instructions) {
        instr->print();
    }
    std::cout << "\"]\n";
    if (trueExit) {
        std::cout << "\"]\n";
        std::cout << name << " -> " << trueExit->name << " ";
        std::cout << "[xlabel=\"true\"];\n";
        int n = ++count;
        trueExit->printGraphviz(n);
    }
    if (falseExit) {
        std::cout << name << " -> " << falseExit->name << " ";
        std::cout << "[xlabel=\"false\"];\n";
        int n = ++count;
        trueExit->printGraphviz(n);
    }
}

void BBlock::addInstruction(Tac *ptr) { instructions.push_back(ptr); }
