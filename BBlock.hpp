#ifndef BBLOCK_HPP
#define BBLOCK_HPP

#include <memory>
#include <vector>

#include "Bytecode.hpp"
#include "Tac.hpp"

class BBlock {
  private:
    std::string name;
    std::vector<std::unique_ptr<Tac>> instructions;
    BBlock *trueExit = nullptr;
    BBlock *falseExit = nullptr;
    bool visited = false;

  public:
    BBlock(const std::string &name_) : name(name_){};

    [[nodiscard]] const auto &getName() const { return name; }

    void setTrueBlock(BBlock *ptr) { trueExit = ptr; }
    void setFalseBlock(BBlock *ptr) { falseExit = ptr; }

    [[nodiscard]] bool hasTrueBlock() const { return trueExit != nullptr; };
    [[nodiscard]] bool hasFalseBlock() const { return falseExit != nullptr; };

    BBlock *getTrueBlock() { return trueExit; }
    BBlock *getFalseBlock() { return falseExit; }

    void addInstruction(Tac *ptr);

    void printBlockGraphviz(std::ostream &os);

    [[nodiscard]] bool isVisited() const { return visited; }
    void markVisited() { visited = true; };

    // void generateBytecode(BytecodeMethod &method);
};

#endif
