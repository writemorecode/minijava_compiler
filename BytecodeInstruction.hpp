#ifndef BYTECODE_INSTRUCTION_HPP
#define BYTECODE_INSTRUCTION_HPP

#include <ostream>
#include <vector>

enum Opcode {
    LOAD = 0,
    CONST = 1,
    STORE = 2,
    ADD = 3,
    SUB = 4,
    MUL = 5,
    DIV = 6,
    LT = 7,
    GT = 8,
    EQ = 9,
    AND = 10,
    OR = 11,
    NOT = 12,
    JMP = 13,
    CJMP = 14,
    CALL = 15,
    RET = 16,
    PRINT = 17,
    STOP = 18
};

const std::vector<std::string> mnemonics{
    "ILOAD",        "ICONST",        "ISTORE",  "IADD",  "ISUB", "IMUL", "IDIV",
    "ILT",          "IGT",           "IEQ",     "IAND",  "IOR",  "INOT", "GOTO",
    "IFFALSE GOTO", "INVOKEVIRTUAL", "IRETURN", "PRINT", "STOP"};

class BytecodeInstruction {
  protected:
    Opcode opcode;

  public:
    BytecodeInstruction(Opcode opcode_) : opcode(opcode_){};
    virtual ~BytecodeInstruction() = default;
    virtual void print(std::ostream &os) const = 0;
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
