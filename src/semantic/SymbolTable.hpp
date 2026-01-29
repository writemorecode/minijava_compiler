#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <memory>

#include "semantic/Class.hpp"
#include "semantic/Scope.hpp"

class SymbolTable {
  private:
    std::unique_ptr<Scope> root;
    Scope *current;

  public:
    SymbolTable();

    void enterScope(const std::string &name, Record *record = nullptr);
    void enterClassScope(Class *scopeClass);
    void enterClassScope(const std::string &scopeName,
                         Record *record = nullptr);
    void enterMethodScope(Method *scopeMethod);
    void enterMethodScope(const std::string &scopeName,
                          Record *record = nullptr);

    void exitScope();

    void addVariable(const std::string &type, const std::string &id);
    void addMethod(const std::string &type, const std::string &id);
    void addClass(const std::string &id);

    void addIntegerVariable(const std::string &id);
    void addBooleanVariable(const std::string &id);

    [[nodiscard]] Variable *lookupVariable(const std::string &id);
    [[nodiscard]] Method *lookupMethod(const std::string &id);
    [[nodiscard]] Class *lookupClass(const std::string &id);

    [[nodiscard]] Variable *lookupVariableInScope(const std::string &key);

    [[nodiscard]] Record *getCurrentRecord() const;

    std::string getCurrentScopeName() const;
    [[nodiscard]] Scope *getParentScope() const;
    [[nodiscard]] Scope *getCurrentScope() const;

    void printTable(std::ostream &os) const;

    [[nodiscard]] Scope *resolveScope(const std::string &className,
                                      const std::string &methodName);
};

#endif
