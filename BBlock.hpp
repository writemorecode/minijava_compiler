#ifndef BBLOCK_HPP
#define BBLOCK_HPP

#include <memory>
#include <vector>

#include "Tac.hpp"

class BBlock {
  private:
    std::string name;
    std::vector<std::unique_ptr<Tac>> instructions;
    Tac *condition = nullptr;
    BBlock *trueExit = nullptr;
    BBlock *falseExit = nullptr;

  public:
    BBlock(std::string name_) : name(name_){};

    const auto &getName() const { return name; }

    void setTrueBlock(BBlock *ptr) { trueExit = ptr; }
    void setFalseBlock(BBlock *ptr) { falseExit = ptr; }

    void addInstruction(Tac *ptr);

    void printGraphviz() const;
};

#endif
