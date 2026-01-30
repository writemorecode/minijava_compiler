#ifndef BYTECODEMAINMETHOD_HPP
#define BYTECODEMAINMETHOD_HPP

#include "bytecode/BytecodeMethodBlock.hpp"
#include <string>

class BytecodeMainMethod {
    std::string name;
    BytecodeMethodBlock block;

  public:
    BytecodeMainMethod(const std::string &name_) : name(name_), block(name) {};

    BytecodeMethodBlock &getBlock() { return block; }

    const std::string &getName() const { return name; }

    void print(std::ostream &os) const;
};

#endif // BYTECODEMETHOD_HPP
