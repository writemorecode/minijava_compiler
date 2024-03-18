#ifndef LOGICAL_TAC_HPP
#define LOGICAL_TAC_HPP

#include "Tac.hpp"

class LessThanTac : public Tac {
  public:
    LessThanTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), "<", std::move(z_)){};
};

class GreaterThanTac : public Tac {
  public:
    GreaterThanTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), ">", std::move(z_)){};
};

class EqualToTac : public Tac {
  public:
    EqualToTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), "==", std::move(z_)){};
};

#endif
