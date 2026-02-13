#ifndef BBLOCK_HPP
#define BBLOCK_HPP

#include <memory>
#include <vector>

#include "bytecode/BytecodeMethod.hpp"
#include "ir/Tac.hpp"

class BBlock {
  private:
    std::string className, methodName;
    std::string name;
    std::vector<std::unique_ptr<Tac>> instructions;
    BBlock *trueExit = nullptr;
    BBlock *falseExit = nullptr;
    bool visited = false;
    bool generated = false;

  public:
    BBlock(const std::string &className_, const std::string &methodName_)
        : className(className_), methodName(methodName_),
          name(className + "." + methodName) {};
    BBlock(const std::string &name_) : name(name_) {};

    [[nodiscard]] const auto &getName() const { return name; }
    [[nodiscard]] const auto &getClassName() const { return className; }
    [[nodiscard]] const auto &getMethodName() const { return methodName; }

    void setTrueBlock(BBlock *ptr) { trueExit = ptr; }
    void setFalseBlock(BBlock *ptr) { falseExit = ptr; }

    [[nodiscard]] bool hasTrueBlock() const { return trueExit != nullptr; };
    [[nodiscard]] bool hasFalseBlock() const { return falseExit != nullptr; };

    BBlock *getTrueBlock() { return trueExit; }
    BBlock *getFalseBlock() { return falseExit; }

    void addInstruction(Tac *ptr);
    [[nodiscard]] const auto &getInstructions() const { return instructions; }
    [[nodiscard]] auto &getInstructions() { return instructions; }

    void printBlockGraphviz(std::ostream &os);

    [[nodiscard]] bool isVisited() const { return visited; }
    void markVisited() { visited = true; };
    void resetVisited() { visited = false; };

    [[nodiscard]] bool isGenerated() const { return generated; }
    void markGenerated() { generated = true; };
    void resetGenerated() { generated = false; };

    void generateBytecode(BytecodeMethod &method);
};

#endif
