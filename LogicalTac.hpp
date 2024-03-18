#ifndef LOGICAL_TAC_HPP
#define LOGICAL_TAC_HPP

#include "Tac.hpp"

class LessThanTac : public Tac {
  public:
    LessThanTac(const std::string &result_, const std::string &y_,
                const std::string &z_)
        : Tac(result_, y_, "<", z_){};
};

class GreaterThanTac : public Tac {
  public:
    GreaterThanTac(const std::string &result_, const std::string &y_,
                   const std::string &z_)
        : Tac(result_, y_, ">", z_){};
};

class EqualToTac : public Tac {
  public:
    EqualToTac(const std::string &result_, const std::string &y_,
               const std::string &z_)
        : Tac(result_, y_, "==", z_){};
};

#endif
