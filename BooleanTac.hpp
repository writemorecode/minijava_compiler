#ifndef BOOLEAN_TAC_HPP
#define BOOLEAN_TAC_HPP

#include "Tac.hpp"

class AndTac : public Tac {
  public:
    AndTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), "&&", std::move(z_)){};
};

class OrTac : public Tac {
  public:
    OrTac(std::string result_, std::string y_, std::string z_)
        : Tac(std::move(result_), std::move(y_), "||", std::move(z_)){};
};

#endif
