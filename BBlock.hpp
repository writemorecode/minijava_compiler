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
    bool visited = false;

  public:
    BBlock(std::string name_) : name(name_){};

    const auto &getName() const { return name; }

    void setTrueBlock(BBlock *ptr) { trueExit = ptr; }
    void setFalseBlock(BBlock *ptr) { falseExit = ptr; }

    bool hasTrueBlock() const { return trueExit != nullptr; };
    bool hasFalseBlock() const { return falseExit != nullptr; };

    void addInstruction(Tac *ptr);

    void printBlockGraphviz();

    bool isVisited() const { return visited; }
    void markVisited() { visited = true; };
};

#endif
