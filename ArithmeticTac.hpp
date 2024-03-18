#ifndef ARITHMETIC_TAC_HPP
#define ARITHMETIC_TAC_HPP

#include "Tac.hpp"

class AddTac : public Tac {
  public:
    AddTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), "+", std::move(z_)){};
};

class SubtractTac : public Tac {
  public:
    SubtractTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), "-", std::move(z_)){};
};

class MultiplyTac : public Tac {
  public:
    MultiplyTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), "*", std::move(z_)){};
};

class DivideTac : public Tac {
  public:
    DivideTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), "/", std::move(z_)){};
};

#endif
