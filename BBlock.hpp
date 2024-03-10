#ifndef BBLOCK_HPP
#define BBLOCK_HPP

#include <vector>

#include "Tac.hpp"

class BBlock {
  private:
    std::string name;
    std::vector<Tac *> instructions;
    Tac *condition = nullptr;
    BBlock *trueExit = nullptr;
    BBlock *falseExit = nullptr;

  public:
    BBlock(std::string name_) : name(name_){};

    void setTrueBlock(BBlock *ptr) { trueExit = ptr; }
    void setFalseBlock(BBlock *ptr) { falseExit = ptr; }

    void addInstruction(Tac *ptr);

    void printGraphviz(int &count) const;
};

#endif
