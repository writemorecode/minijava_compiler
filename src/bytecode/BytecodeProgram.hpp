#ifndef BYTECODE_HPP
#define BYTECODE_HPP

#include <string>
#include <vector>

#include "bytecode/BytecodeMethod.hpp"

class BytecodeProgram {
    std::vector<BytecodeMethod> methods;

  public:
    [[nodiscard]] BytecodeMethod &
    addBytecodeMethod(const std::string &name,
                      std::vector<std::string> variables,
                      std::vector<std::string> fieldVariables);

    [[nodiscard]] BytecodeMethod &getBytecodeMethod(const std::string &name);
    [[nodiscard]] std::vector<const BytecodeInstruction *>
    getInstructions() const;

    void print(std::ostream &os) const;

    void serialize(std::ofstream &os) const;
};

#endif
