#ifndef BYTECODE_INSTRUCTION_HPP
#define BYTECODE_INSTRUCTION_HPP

#include <ostream>
#include <vector>

#include "Opcode.hpp"

const std::vector<std::string> mnemonics{
    "ILOAD",        "ICONST",        "ISTORE",  "IADD",  "ISUB", "IMUL", "IDIV",
    "ILT",          "IGT",           "IEQ",     "IAND",  "IOR",  "INOT", "GOTO",
    "IFFALSE GOTO", "INVOKEVIRTUAL", "IRETURN", "PRINT", "STOP"};

class BytecodeInstruction {
  protected:
    Opcode opcode;
    std::string mnemonic;

  public:
    BytecodeInstruction(Opcode opcode_)
        : opcode(opcode_), mnemonic(mnemonics[opcode]){};
    virtual ~BytecodeInstruction() = default;
    virtual void print(std::ostream &os) const = 0;

    Opcode getOpcode() const { return opcode; }
};

class StackParameterInstruction : public BytecodeInstruction {
    // An instruction which pops two parameters from the stack and pushes
    // one result back to the stack
  public:
    StackParameterInstruction(Opcode opcode_) : BytecodeInstruction(opcode_){};
    void print(std::ostream &os) const override;
};

class IntegerParameterInstruction : public BytecodeInstruction {
    // An instruction which takes one integer parameter
    // and pushes one result back to the stack
    int param;

  public:
    IntegerParameterInstruction(Opcode opcode_, int param_)
        : BytecodeInstruction(opcode_), param(param_){};
    void print(std::ostream &os) const override;
};

class StringParameterInstruction : public BytecodeInstruction {
    // An instruction which takes one integer parameter
    // and pushes one result back to the stack
    std::string param;

  public:
    StringParameterInstruction(Opcode opcode_, const std::string &param_)
        : BytecodeInstruction(opcode_), param(param_){};
    void print(std::ostream &os) const override;
};

#endif
