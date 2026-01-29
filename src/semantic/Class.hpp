#ifndef CLASS_HPP
#define CLASS_HPP

#include <string>
#include <unordered_map>

#include "semantic/Method.hpp"
#include "semantic/Record.hpp"
#include "semantic/Variable.hpp"

class Class : public Record {
    /*
     * We use pointers here, since the Variable, Method, etc. objects are stored
     * (and owned) as unique_ptrs to Records in the "records" unordered_map in
     * the SymbolTable class.
     * */
    std::unordered_map<std::string, Variable *> variables;
    std::unordered_map<std::string, Method *> methods;

  public:
    Class(const std::string &id) : Record(id, id){};

    void addVariable(Variable *variable);
    void addMethod(Method *method);

    Variable *lookupVariable(const std::string &id);
    Method *lookupMethod(const std::string &id);

    std::string getRecord() const override;
};

#endif
