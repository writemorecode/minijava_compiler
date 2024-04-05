#ifndef BYTECODE_HPP
#define BYTECODE_HPP

#include <string>
#include <unordered_map>

#include "BytecodeMainMethod.hpp"
#include "BytecodeMethod.hpp"

#include "SymbolTable.hpp"

class BytecodeProgram {
    std::unordered_map<std::string, BytecodeMethod> methods;

  public:
    [[nodiscard]] BytecodeMethod &addBytecodeMethod(const std::string &name);

    [[nodiscard]] BytecodeMethod &getBytecodeMethod(const std::string &name);

    void print(std::ostream &os) const;

    void serialize(SymbolTable &st, std::ofstream &os) const;
};

#endif
