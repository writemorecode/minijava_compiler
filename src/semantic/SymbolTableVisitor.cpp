#include "semantic/SymbolTableVisitor.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>

#include "ast/ClassNode.hpp"
#include "ast/MainClassNode.hpp"
#include "ast/MethodNode.hpp"
#include "ast/MethodParameterNode.hpp"
#include "ast/MethodWithoutParametersNode.hpp"
#include "ast/Node.h"
#include "ast/VariableNode.hpp"
#include "semantic/Class.hpp"
#include "semantic/Method.hpp"
#include "semantic/Record.hpp"
#include "semantic/Variable.hpp"

namespace {

class ScopeExit {
  public:
    explicit ScopeExit(SymbolTable &table) : table_(&table) {}

    ~ScopeExit() {
        if (table_ != nullptr) {
            table_->exitScope();
        }
    }

    ScopeExit(const ScopeExit &) = delete;
    ScopeExit &operator=(const ScopeExit &) = delete;

  private:
    SymbolTable *table_ = nullptr;
};

} // namespace

void SymbolTableVisitor::emit_error(int line, std::string message) {
    error_count_ += 1;

    if (sink_ == nullptr) {
        return;
    }

    const auto line_no = static_cast<std::size_t>(std::max(line, 1));
    const lexing::SourceSpan span{
        .begin = {.offset = 0, .line = line_no, .column = 1},
        .end = {.offset = 0, .line = line_no, .column = 1},
    };

    sink_->emit({.severity = lexing::Severity::Error,
                 .message = std::move(message),
                 .span = span});
}

void SymbolTableVisitor::visit(const Node &node) {
    for (const auto &child : node.children) {
        child->accept(*this);
    }
}

void SymbolTableVisitor::visit(const ClassNode &node) {
    const auto &class_name = node.getClassName();

    if (table_.lookupClass(class_name) != nullptr) {
        emit_error(node.lineno, "Error: (line " + std::to_string(node.lineno) +
                                   ") Class '" + class_name +
                                   "' already declared.\n");
        return;
    }

    table_.addClass(class_name);
    auto *current_class = table_.lookupClass(class_name);
    table_.enterClassScope(current_class);
    ScopeExit exit_scope(table_);

    table_.addVariable(class_name, "this");
    auto *this_variable = table_.lookupVariableInScope("this");
    current_class->addVariable(this_variable);

    node.getBodyNode().accept(*this);
}

void SymbolTableVisitor::visit(const MainClassNode &node) {
    const auto &main_class_name = node.getMainClassName();

    if (table_.lookupClass(main_class_name) != nullptr) {
        emit_error(node.lineno,
                   "Error: (line " + std::to_string(node.lineno) +
                       ") Class '" + main_class_name +
                       "' already declared.\n");
        return;
    }

    table_.addClass(main_class_name);
    auto *main_class = table_.lookupClass(main_class_name);
    table_.enterClassScope(main_class);
    ScopeExit exit_class_scope(table_);

    table_.addVariable(main_class_name, "this");
    auto *main_class_this = table_.lookupVariableInScope("this");
    main_class->addVariable(main_class_this);

    table_.addMethod("void", "main");
    auto *main_class_method = table_.lookupMethod("main");
    main_class->addMethod(main_class_method);

    table_.enterMethodScope(main_class_method);
    {
        ScopeExit exit_method_scope(table_);
        table_.addVariable("String[]", node.getMainMethodArgumentName());
    }

    node.getBodyNode().accept(*this);
}

void SymbolTableVisitor::visit(const MethodNode &node) {
    auto *current_class = dynamic_cast<Class *>(table_.getCurrentRecord());
    const auto &method_name = node.getMethodName();

    if (current_class != nullptr && current_class->lookupMethod(method_name)) {
        emit_error(node.lineno,
                   "Error: (line " + std::to_string(node.lineno) +
                       ") Method '" + method_name + "' already declared.\n");
        return;
    }

    table_.addMethod(node.getMethodType(), method_name);
    auto *current_method = table_.lookupMethod(method_name);

    if (current_class != nullptr) {
        current_class->addMethod(current_method);
    }

    table_.enterMethodScope(current_method);
    ScopeExit exit_method_scope(table_);

    node.getParametersNode().accept(*this);
    node.getBodyNode().accept(*this);
}

void SymbolTableVisitor::visit(const MethodWithoutParametersNode &node) {
    auto *current_class = dynamic_cast<Class *>(table_.getCurrentRecord());
    const auto &method_name = node.getMethodName();

    if (current_class != nullptr && current_class->lookupMethod(method_name)) {
        emit_error(node.lineno,
                   "Error: (line " + std::to_string(node.lineno) +
                       ") Method '" + method_name + "' already declared.\n");
        return;
    }

    table_.addMethod(node.getMethodType(), method_name);
    auto *current_method = table_.lookupMethod(method_name);

    if (current_class != nullptr) {
        current_class->addMethod(current_method);
    }

    table_.enterMethodScope(current_method);
    ScopeExit exit_method_scope(table_);

    node.getBodyNode().accept(*this);
}

void SymbolTableVisitor::visit(const MethodParameterNode &node) {
    const auto &parameter_name = node.getParameterName();

    if (table_.lookupVariableInScope(parameter_name) != nullptr) {
        emit_error(node.lineno,
                   "Error: (line " + std::to_string(node.lineno) +
                       ") Parameter '" + parameter_name +
                       "' already declared.\n");
        return;
    }

    table_.addVariable(node.getParameterType(), parameter_name);
    auto *parameter = table_.lookupVariable(parameter_name);

    auto *current_scope = table_.getCurrentScope();
    auto *current_method =
        dynamic_cast<Method *>(current_scope != nullptr ? current_scope->getRecord()
                                                        : nullptr);
    if (current_method != nullptr) {
        current_method->addParameter(parameter);
    }
}

void SymbolTableVisitor::visit(const VariableNode &node) {
    const auto &variable_name = node.getVariableName();

    if (table_.lookupVariableInScope(variable_name) != nullptr) {
        emit_error(node.lineno,
                   "Error: (line " + std::to_string(node.lineno) +
                       ") Variable '" + variable_name +
                       "' already declared.\n");
        return;
    }

    table_.addVariable(node.getVariableType(), variable_name);
    auto *current_variable = table_.lookupVariable(variable_name);

    auto *current_record = table_.getCurrentRecord();
    if (auto *current_class = dynamic_cast<Class *>(current_record)) {
        current_class->addVariable(current_variable);
        return;
    }

    if (auto *current_method = dynamic_cast<Method *>(current_record)) {
        current_method->addVariable(current_variable);
    }
}

SemanticPassResult build_symbol_table(const Node &root, SymbolTable &table,
                                      lexing::DiagnosticSink *sink) {
    while (table.getParentScope() != nullptr) {
        table.exitScope();
    }

    SymbolTableVisitor visitor(table, sink);
    root.accept(visitor);
    return visitor.result();
}
