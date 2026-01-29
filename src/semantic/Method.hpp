#ifndef METHOD_HPP
#define METHOD_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include "semantic/Record.hpp"
#include "semantic/Variable.hpp"

class Method : public Record {
    std::unordered_map<std::string, Variable *> variables;
    std::vector<Variable *> parameters;

  public:
    Method(const std::string &type, const std::string &id) : Record(id, type){};

    std::string getRecord() const override;

    void addVariable(Variable *variable);
    void addParameter(Variable *parameter);

    size_t getParameterCount() const;

    const std::vector<Variable *> &getParameters() const;
    [[nodiscard]] std::vector<std::string> getParameterNames() const;
};

#endif
