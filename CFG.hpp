#ifndef CFG_HPP
#define CFG_HPP

#include <vector>

#include "BBlock.hpp"
#include "BytecodeProgram.hpp"
#include "SymbolTable.hpp"

class CFG {
  private:
    BBlock *currentBlock = nullptr;
    std::vector<BBlock *> methodBlocks;
    int temporaryIndex = 0;
    int blockIndex = 0;

  public:
    std::string getTemporaryName();
    std::string getBlockName();

    BBlock *getCurrentBlock() const { return currentBlock; }
    void setCurrentBlock(BBlock *ptr) { currentBlock = ptr; }

    void addInstruction(Tac *ptr);

    void printGraphviz(std::ostream &os) const;

    [[nodiscard]] BBlock *newBlock();
    [[nodiscard]] BBlock *addMethodBlock();
    [[nodiscard]] BBlock *addMethodBlock(const std::string &name);

    void generateBytecode(BytecodeProgram &program, SymbolTable &st);
};

#endif
