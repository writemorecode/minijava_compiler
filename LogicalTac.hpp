#ifndef LOGICAL_TAC_HPP
#define LOGICAL_TAC_HPP

#include "Tac.hpp"

class LessThanTac : public Tac {
  public:
    LessThanTac(const std::string &result_, const Operand &y_,
                const Operand &z_)
        : Tac(result_, y_, "<", z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class GreaterThanTac : public Tac {
  public:
    GreaterThanTac(const std::string &result_, const Operand &y_,
                   const Operand &z_)
        : Tac(result_, y_, ">", z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class EqualToTac : public Tac {
  public:
    EqualToTac(const std::string &result_, const Operand &y_, const Operand &z_)
        : Tac(result_, y_, "==", z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
};

#endif
