#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Class.hpp"
#include "Method.hpp"
#include "Record.hpp"
#include "Variable.hpp"

class Scope {
  private:
    unsigned int next = 0;
    std::vector<std::unique_ptr<Scope>> children;

    std::unordered_map<std::string, Variable> variables;
    std::unordered_map<std::string, Method> methods;
    std::unordered_map<std::string, Class> classes;

    Record *record = nullptr;
    Scope *parent = nullptr;
    std::string scopeName;

  public:
    Scope(Record *record = nullptr, Scope *parent = nullptr,
          const std::string &name = "Program");

    Scope *nextChild(const std::string &name, Record *record);
    Scope *getParent();

    void addVariable(const std::string &type, const std::string &id);
    void addMethod(const std::string &type, const std::string &id);
    void addClass(const std::string &id);

    Variable *lookupVariable(const std::string &key);
    Variable *lookupVariableInScope(const std::string &key);
    Method *lookupMethod(const std::string &key);
    Class *lookupClass(const std::string &key);

    void resetScope();
    void printScope(int &count, std::ostream &os) const;

    std::string getName() const;
    Record *getRecord() const;
};

#endif
