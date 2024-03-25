#ifndef BYTECODEMETHOD_HPP
#define BYTECODEMETHOD_HPP

#include "BytecodeMethodBlock.hpp"
#include <string>
#include <vector>

class BytecodeMethod {
    std::vector<BytecodeMethodBlock> blocks;

  public:
    BytecodeMethod() = default;
    BytecodeMethodBlock &addMethodBlock(const std::string &name);

    BytecodeMethodBlock &getFirstBlock();

    void print(std::ostream &os) const;
};

#endif // BYTECODEMETHOD_HPP
