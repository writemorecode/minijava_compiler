#include "Method.hpp"

void Method::addVariable(Variable* variable)
{
    const auto id = variable->getID();
    variables.insert({ id, variable });
}

void Method::addParameter(Variable* parameter)
{
    parameters.push_back(parameter);
}

std::string Method::getRecord() const { return "Method"; }

size_t Method::getParameterCount() const { return parameters.size(); }

const std::vector<Variable*>& Method::getParameters() const
{
    return parameters;
}
