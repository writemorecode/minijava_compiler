#ifndef IR_GENERATION_VISITOR_HPP
#define IR_GENERATION_VISITOR_HPP

#include <string>
#include <unordered_map>

#include "ast/AstVisitor.hpp"
#include "ir/Tac.hpp"
#include "lexing/Diagnostics.hpp"

class CFG;
class Node;
class SymbolTable;

class ClassNode;
class MainClassNode;
class MethodNode;
class MethodWithoutParametersNode;

struct IRGenerationResult {
    int error_count = 0;

    [[nodiscard]] bool ok() const { return error_count == 0; }
};

class IRGenerationVisitor : public AstVisitor {
  public:
    explicit IRGenerationVisitor(CFG &graph, SymbolTable &table,
                                 lexing::DiagnosticSink *sink = nullptr)
        : graph_(graph), table_(table), sink_(sink) {}

    [[nodiscard]] IRGenerationResult result() const {
        return {.error_count = error_count_};
    }

    void visit(const Node &node) override;
    void visit(const ClassNode &node) override;
    void visit(const MainClassNode &node) override;
    void visit(const MethodNode &node) override;
    void visit(const MethodWithoutParametersNode &node) override;

  private:
    CFG &graph_;
    SymbolTable &table_;
    lexing::DiagnosticSink *sink_ = nullptr;
    int error_count_ = 0;
    std::unordered_map<const Node *, Operand> values_;

    [[nodiscard]] Operand eval(const Node &node);
    void set_value(const Node &node, Operand value);
    void visit_generic(const Node &node);
    void emit_error(int line, std::string message);
};

IRGenerationResult generate_ir(const Node &root, CFG &graph, SymbolTable &table,
                               lexing::DiagnosticSink *sink = nullptr);

#endif
