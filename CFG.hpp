#ifndef CFG_HPP
#define CFG_HPP

#include "BBlock.hpp"

class CFG {
  private:
    BBlock *currentBlock = nullptr;
    BBlock *rootBlock = nullptr;
    int temporaryIndex = 0;
    int blockIndex = 1;

  public:
    CFG() {
        currentBlock = new BBlock("block_0");
        rootBlock = currentBlock;
    };

    std::string getTemporaryName();
    std::string getBlockName();

    BBlock *getCurrentBlock() const { return currentBlock; }
    BBlock *getRootBlock() const { return rootBlock; }
    void setCurrentBlock(BBlock *ptr) { currentBlock = ptr; }

    void addInstruction(Tac *ptr);

    void printGraphviz() const;

    [[nodiscard]] BBlock *newBlock();
};

#endif
