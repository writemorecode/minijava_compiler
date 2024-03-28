#ifndef BYTECODEMETHOD_HPP
#define BYTECODEMETHOD_HPP

#include "BytecodeMethodBlock.hpp"
#include <string>
#include <vector>

class BytecodeMethod {
    std::vector<BytecodeMethodBlock> blocks;
    std::string name;

  public:
    BytecodeMethod(const std::string &name_) : name(name_){};

    [[nodiscard]] BytecodeMethodBlock &
    addBytecodeMethodBlock(const std::string &name);

    [[nodiscard]] BytecodeMethodBlock &
    getBytecodeMethodBlock(const std::string &name);

    BytecodeMethodBlock &getFirstBlock();

    void print(std::ostream &os) const;
};

#endif // BYTECODEMETHOD_HPP
