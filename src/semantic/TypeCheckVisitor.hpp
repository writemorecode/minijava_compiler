#ifndef TYPE_CHECK_VISITOR_HPP
#define TYPE_CHECK_VISITOR_HPP

#include <string>
#include <unordered_map>

#include "lexing/Diagnostics.hpp"

class Node;
class SymbolTable;

struct TypeCheckResult {
    int error_count = 0;

    [[nodiscard]] bool ok() const { return error_count == 0; }
};

class TypeInfo {
  public:
    void set(const Node &node, std::string type_name);
    [[nodiscard]] const std::string *get(const Node &node) const;

  private:
    std::unordered_map<const Node *, std::string> map_;
};

TypeCheckResult check_types(const Node &root, SymbolTable &table,
                            TypeInfo *type_info,
                            lexing::DiagnosticSink *sink = nullptr);

#endif
