#ifndef VARIABLE_HPP
#define VARIABLE_HPP

#include <string>

#include "Record.hpp"

class Variable : public Record {
public:
    Variable(std::string type, std::string id)
        : Record(id, type) {};
    virtual ~Variable() = default;
    std::string getRecord() const override;
};

#endif
