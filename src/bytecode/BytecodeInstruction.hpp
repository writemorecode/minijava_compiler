#ifndef BYTECODE_INSTRUCTION_HPP
#define BYTECODE_INSTRUCTION_HPP

#include <cstdint>
#include <ostream>
#include <vector>

#include "bytecode/Opcode.hpp"
#include "util/serialize.hpp"

class BytecodeInstruction {
  protected:
    Opcode opcode;
    std::string mnemonic;

  public:
    BytecodeInstruction(Opcode opcode_)
        : opcode(opcode_), mnemonic(mnemonics.at(static_cast<size_t>(
                               static_cast<std::uint8_t>(opcode_)))) {};
    virtual ~BytecodeInstruction() = default;
    virtual void print(std::ostream &os) const = 0;

    virtual void serialize(Serializer &serializer) const = 0;

    Opcode getOpcode() const { return opcode; }
};

class StackParameterInstruction : public BytecodeInstruction {
    // An instruction which pops two parameters from the stack and pushes
    // one result back to the stack
  public:
    explicit StackParameterInstruction(Opcode opcode_)
        : BytecodeInstruction(opcode_) {};
    void print(std::ostream &os) const override;
    void serialize(Serializer &serializer) const override;
};

class IntegerParameterInstruction : public BytecodeInstruction {
    // An instruction which takes one integer parameter
    // and pushes one result back to the stack
    std::int64_t param;

  public:
    IntegerParameterInstruction(Opcode opcode_, std::int64_t param_)
        : BytecodeInstruction(opcode_), param(param_) {};
    void print(std::ostream &os) const override;
    void serialize(Serializer &serializer) const override;
    [[nodiscard]] std::int64_t getParam() const { return param; }
};

class StringParameterInstruction : public BytecodeInstruction {
    // An instruction which takes one integer parameter
    // and pushes one result back to the stack
    std::string param;

  public:
    StringParameterInstruction(Opcode opcode_, const std::string &param_)
        : BytecodeInstruction(opcode_), param(param_) {};
    void print(std::ostream &os) const override;

    void serialize(Serializer &serializer) const override;
};

#endif
