#ifndef BYTECODE_HPP
#define BYTECODE_HPP

#include <string>
#include <vector>

#include "BytecodeMethod.hpp"

class BytecodeProgram {
    std::vector<BytecodeMethod> methods;

  public:
    [[nodiscard]] BytecodeMethod &
    addBytecodeMethod(const std::string &name,
                      std::vector<std::string> variables);

    [[nodiscard]] BytecodeMethod &getBytecodeMethod(const std::string &name);

    void print(std::ostream &os) const;

    void serialize(std::ofstream &os) const;
};

#endif
