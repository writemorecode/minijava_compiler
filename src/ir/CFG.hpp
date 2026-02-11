#ifndef CFG_HPP
#define CFG_HPP

#include <memory>
#include <vector>

#include "bytecode/BytecodeProgram.hpp"
#include "ir/BBlock.hpp"
#include "semantic/SymbolTable.hpp"

class Node;
class TypeInfo;

class CFG {
  private:
    BBlock *currentBlock = nullptr;
    std::vector<std::unique_ptr<BBlock>> allBlocks;
    std::vector<BBlock *> methodRoots;
    int temporaryIndex = 0;
    int blockIndex = 0;
    const TypeInfo *type_info_ = nullptr;

    [[nodiscard]] BBlock *ownBlock(std::unique_ptr<BBlock> block);
    void resetVisitedFlags() const;
    void resetGeneratedFlags() const;

  public:
    std::string getTemporaryName();
    std::string getBlockName();

    BBlock *getCurrentBlock() const { return currentBlock; }
    void setCurrentBlock(BBlock *ptr) { currentBlock = ptr; }

    void addInstruction(Tac *ptr);

    void printGraphviz(std::ostream &os) const;

    [[nodiscard]] BBlock *newBlock();
    [[nodiscard]] BBlock *addMethodBlock();
    [[nodiscard]] BBlock *addMethodRootBlock(const std::string &className,
                                             const std::string &methodName);

    void setTypeInfo(const TypeInfo *info) { type_info_ = info; }
    [[nodiscard]] const std::string *typeOf(const Node &node) const;

    void generateBytecode(BytecodeProgram &program, SymbolTable &st);
};

#endif
