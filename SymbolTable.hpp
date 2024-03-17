#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <memory>

#include "Class.hpp"
#include "Scope.hpp"

class SymbolTable {
  private:
    std::unique_ptr<Scope> root;
    Scope *current;

  public:
    SymbolTable();

    void enterScope(const std::string &name, Record *record = nullptr);
    void exitScope();

    void addVariable(const std::string &type, const std::string &id);
    void addMethod(const std::string &type, const std::string &id);
    void addClass(const std::string &id);

    void addIntegerVariable(const std::string &id);

    Variable *lookupVariable(const std::string &id);
    Method *lookupMethod(const std::string &id);
    Class *lookupClass(const std::string &id);

    Variable *lookupVariableInScope(const std::string &key);

    Record *getCurrentRecord() const;

    std::string getCurrentScopeName() const;
    Scope *getParentScope() const;
    Scope *getCurrentScope() const;

    void printTable(std::ostream &os) const;
    void resetTable();
};

#endif
