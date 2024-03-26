#ifndef BYTECODE_METHOD_BLOCK_HPP
#define BYTECODE_METHOD_BLOCK_HPP

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "BytecodeInstruction.hpp"

class BytecodeMethodBlock {
    std::vector<std::unique_ptr<BytecodeInstruction>> instructions;
    std::string name;

  public:
    BytecodeMethodBlock(const std::string &name_) : name(name_){};
    std::string getName() const { return name; }
    void print(std::ostream &os) const;
    void addBytecodeInstruction(BytecodeInstruction *instr);

    void
    addOperandPushInstruction(const std::variant<std::string, int> &operand);

    void addStoreInstruction(const std::string &result);
};

#endif
