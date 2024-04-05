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
    std::unordered_map<std::string, std::unique_ptr<Scope>> children;

    std::unordered_map<std::string, Variable> variables;
    std::unordered_map<std::string, Method> methods;
    std::unordered_map<std::string, Class> classes;

    std::string scopeName = "Program";
    Record *record = nullptr;
    Scope *parent = nullptr;

  public:
    Scope() = default;
    Scope(const std::string &name) : scopeName(name){};
    Scope(const std::string &name, Record *record_)
        : scopeName(name), record(record_){};
    Scope(const std::string &name, Record *record_, Scope *parent_)
        : scopeName(name), record(record_), parent(parent_){};

    Scope *nextChild(const std::string &name, Record *record);
    Scope *getParent();

    void addVariable(const std::string &type, const std::string &id);
    void addMethod(const std::string &type, const std::string &id);
    void addClass(const std::string &id);

    Variable *lookupVariable(const std::string &key);
    Variable *lookupVariableInScope(const std::string &key);
    Method *lookupMethod(const std::string &key);
    Class *lookupClass(const std::string &key);

    void printScope(int &count, std::ostream &os) const;

    std::string getName() const;
    Record *getRecord() const;

    std::vector<std::string> getVariableNames() const;
};

#endif
