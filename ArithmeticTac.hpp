#ifndef ARITHMETIC_TAC_HPP
#define ARITHMETIC_TAC_HPP

#include "Tac.hpp"

class AddTac : public Tac {
  public:
    AddTac(const std::string &result_, const std::string &y_,
           const std::string &z_)
        : Tac(result_, y_, "+", z_){};
};

class SubtractTac : public Tac {
  public:
    SubtractTac(const std::string &result_, const std::string &y_,
                const std::string &z_)
        : Tac(result_, y_, "-", z_){};
};

class MultiplyTac : public Tac {
  public:
    MultiplyTac(const std::string &result_, const std::string &y_,
                const std::string &z_)
        : Tac(result_, y_, "*", z_){};
};

class DivideTac : public Tac {
  public:
    DivideTac(const std::string &result_, const std::string &y_,
              const std::string &z_)
        : Tac(result_, y_, "/", z_){};
};

#endif
