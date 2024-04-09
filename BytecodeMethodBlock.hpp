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
    bool operator==(const std::string &rhsName) { return name == rhsName; };

    BytecodeMethodBlock(const std::string &name_) : name(name_){};
    [[nodiscard]] const std::string &getName() const { return name; }
    void print(std::ostream &os) const;
    void addBytecodeInstruction(BytecodeInstruction *instr);

    BytecodeMethodBlock &push(const std::variant<std::string, int> &operand);
    BytecodeMethodBlock &store(const std::string &result);

    BytecodeMethodBlock &add();
    BytecodeMethodBlock &subtract();
    BytecodeMethodBlock &multiply();
    BytecodeMethodBlock &divide();
    BytecodeMethodBlock &less_than();
    BytecodeMethodBlock &greater_than();
    BytecodeMethodBlock &equal_to();
    BytecodeMethodBlock &l_and();
    BytecodeMethodBlock &l_or();
    BytecodeMethodBlock &l_not();
    BytecodeMethodBlock &ret();

    BytecodeMethodBlock &write();

    BytecodeMethodBlock &call(const std::string &method);

    BytecodeMethodBlock &jump(const std::string &location);
    BytecodeMethodBlock &cjump(const std::string &location);

    BytecodeMethodBlock &stop();
};

#endif
