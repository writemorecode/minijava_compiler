#ifndef BYTECODE_HPP
#define BYTECODE_HPP

#include <string>
#include <unordered_map>

#include "BytecodeMethod.hpp"

class BytecodeProgram {
    std::unordered_map<std::string, BytecodeMethod> methods;

  public:
    void addMethod(const std::string &name);
    [[nodiscard]] BytecodeMethod &getMethod(const std::string &name);

    void print(std::ostream &os) const;
};

#endif
