#ifndef CFG_HPP
#define CFG_HPP

#include "BBlock.hpp"

class CFG {
  private:
    BBlock *currentBlock = nullptr;
    int temporaryIndex = 0;
    int blockIndex = 0;

  public:
    // CFG(){};

    std::string getTemporaryName();
    std::string getBlockName();

    BBlock *getCurrentBlock() const { return currentBlock; }
    void setCurrentBlock(BBlock *ptr) { currentBlock = ptr; }

    void addInstruction(Tac *ptr);

    void printGraphviz() const;

    BBlock *newBlock();
};

#endif
