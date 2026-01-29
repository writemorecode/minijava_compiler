#ifndef BOOLEAN_TAC_HPP
#define BOOLEAN_TAC_HPP

#include "ir/Tac.hpp"

class AndTac : public Tac {
  public:
    AndTac(const std::string &result_, const Operand &y_, const Operand &z_)
        : Tac(result_, y_, "&&", z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
};

class OrTac : public Tac {
  public:
    OrTac(const std::string &result_, const Operand &y_, const Operand &z_)
        : Tac(result_, y_, "||", z_){};
    void generateBytecode(BytecodeMethodBlock &block) override;
};

#endif
