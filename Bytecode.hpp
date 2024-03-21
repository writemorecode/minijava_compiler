#ifndef BYTECODE_HPP
#define BYTECODE_HPP

#include <string>
#include <unordered_map>
#include <vector>

// #include "Method.hpp"
// #include "Variable.hpp"

enum class Opcode : unsigned int {
    ILOAD = 0,
    ICONST = 1,
    ISTORE = 2,
    IADD = 3,
    ISUB = 4,
    IMUL = 5,
    IDIV = 6,
    ILT = 7,
    IAND = 8,
    IOR = 9,
    INOT = 10,
    JMP = 11,
    CJMP = 12,
    CALL = 13,
    RET = 14,
    PRINT = 15,
    STOP = 16
};

class BytecodeInstruction {
    Opcode opcode;

  public:
    BytecodeInstruction(Opcode opcode_) : opcode(opcode_){};

    Opcode getOpcode() const { return opcode; }
};

class BytecodeMethodBlock {
    std::vector<BytecodeInstruction> instructions;
    std::string name;

  public:
    BytecodeMethodBlock(const std::string &name_) : name(name_){};
    std::string getName() const { return name; }
    // void addBytecodeInstruction(Opcode code);
};

class BytecodeMethod {
    // std::vector<Variable> &x;
    std::vector<BytecodeMethodBlock> blocks;

  public:
    BytecodeMethodBlock &addMethodBlock(const std::string &name);
};

class BytecodeProgram {
    std::unordered_map<std::string, BytecodeMethod> methods;

  public:
    [[nodiscard]] BytecodeMethod &addMethod(const std::string &name);
};

#endif
