#ifndef ARITHMETIC_TAC_HPP
#define ARITHMETIC_TAC_HPP

#include "Tac.hpp"

class AddTac : public Tac {
  public:
    AddTac(const std::string &result_, const Operand &y_, const Operand &z_)
        : Tac(result_, y_, "+", z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class SubtractTac : public Tac {
  public:
    SubtractTac(const std::string &result_, const Operand &y_,
                const Operand &z_)
        : Tac(result_, y_, "-", z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class MultiplyTac : public Tac {
  public:
    MultiplyTac(const std::string &result_, const Operand &y_,
                const Operand &z_)
        : Tac(result_, y_, "*", z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class DivideTac : public Tac {
  public:
    DivideTac(const std::string &result_, const Operand &y_, const Operand &z_)
        : Tac(result_, y_, "/", z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
};

#endif
