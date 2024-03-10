#ifndef METHOD_HPP
#define METHOD_HPP

#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#include "Record.hpp"
#include "Variable.hpp"

class Method : public Record {
    std::unordered_map<std::string, Variable *> variables;
    std::vector<Variable *> parameters;

  public:
    Method(std::string type, std::string id) : Record(id, type){};
    // virtual ~Method() = default;

    std::string getRecord() const override;

    void addVariable(Variable *variable);
    void addParameter(Variable *parameter);

    size_t getParameterCount() const;

    const std::vector<Variable *> &getParameters() const;
};

#endif
