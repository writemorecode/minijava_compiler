#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <string>

#include "semantic/Record.hpp"

class Variable : public Record {
  public:
    Variable(const std::string &type, const std::string &id)
        : Record(id, type) {};
    virtual ~Variable() = default;
    std::string getRecord() const override;
};

#endif
