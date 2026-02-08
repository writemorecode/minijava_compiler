#ifndef SYMBOL_TABLE_VISITOR_HPP
#define SYMBOL_TABLE_VISITOR_HPP

#include <string>

#include "ast/AstVisitor.hpp"
#include "lexing/Diagnostics.hpp"
#include "semantic/SymbolTable.hpp"

class Node;
class ClassNode;
class MainClassNode;
class MethodNode;
class MethodWithoutParametersNode;
class MethodParameterNode;
class VariableNode;

struct SemanticPassResult {
    int error_count = 0;

    [[nodiscard]] bool ok() const { return error_count == 0; }
};

class SymbolTableVisitor : public AstVisitor {
  public:
    explicit SymbolTableVisitor(SymbolTable &table,
                                lexing::DiagnosticSink *sink = nullptr)
        : table_(table), sink_(sink) {}

    SemanticPassResult result() const { return {.error_count = error_count_}; }

    void visit(const Node &node) override;
    void visit(const ClassNode &node) override;
    void visit(const MainClassNode &node) override;
    void visit(const MethodNode &node) override;
    void visit(const MethodWithoutParametersNode &node) override;
    void visit(const MethodParameterNode &node) override;
    void visit(const VariableNode &node) override;

  private:
    SymbolTable &table_;
    lexing::DiagnosticSink *sink_ = nullptr;
    int error_count_ = 0;

    void emit_error(int line, std::string message);
};

SemanticPassResult build_symbol_table(const Node &root, SymbolTable &table,
                                      lexing::DiagnosticSink *sink = nullptr);

#endif
