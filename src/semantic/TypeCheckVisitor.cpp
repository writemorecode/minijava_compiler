#include "semantic/TypeCheckVisitor.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "ast/ArithmeticExpressionNode.hpp"
#include "ast/ArrayAccessNode.hpp"
#include "ast/ArrayLengthNode.hpp"
#include "ast/BooleanExpressionNode.hpp"
#include "ast/BooleanNode.hpp"
#include "ast/ClassAllocationNode.hpp"
#include "ast/ClassNode.hpp"
#include "ast/ControlStatementNode.hpp"
#include "ast/IdentifierNode.hpp"
#include "ast/IntegerArrayAllocationNode.hpp"
#include "ast/IntegerNode.hpp"
#include "ast/LogicalExpressionNode.hpp"
#include "ast/MainClassNode.hpp"
#include "ast/MethodBodyNode.hpp"
#include "ast/MethodCallNode.hpp"
#include "ast/MethodCallWithoutArgumentsNode.hpp"
#include "ast/MethodNode.hpp"
#include "ast/MethodParameterNode.hpp"
#include "ast/MethodWithoutParametersNode.hpp"
#include "ast/Node.h"
#include "ast/NotNode.hpp"
#include "ast/StatementNode.hpp"
#include "ast/ThisNode.hpp"
#include "ast/VariableNode.hpp"
#include "semantic/Class.hpp"
#include "semantic/Method.hpp"
#include "semantic/SymbolTable.hpp"
#include "semantic/TypeNode.hpp"

namespace {

constexpr std::string_view kErrorType = "<type-error>";

[[nodiscard]] bool is_error_type(std::string_view type_name) {
    return type_name == kErrorType;
}

[[nodiscard]] std::string error_type() { return std::string{kErrorType}; }

[[nodiscard]] bool is_builtin_type(std::string_view type_name) {
    return type_name == "int" || type_name == "boolean" || type_name == "int[]";
}

[[nodiscard]] const Node *child_at(const Node &node, std::size_t index) {
    if (index >= node.children.size()) {
        return nullptr;
    }
    auto it = node.children.cbegin();
    std::advance(it, static_cast<long>(index));
    return it->get();
}

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

class TypeCheckVisitor {
  public:
    TypeCheckVisitor(SymbolTable &table, TypeInfo &type_info,
                     lexing::DiagnosticSink *sink)
        : table_(table), type_info_(type_info), sink_(sink) {}

    [[nodiscard]] TypeCheckResult run(const Node &root) {
        (void)visit(root);
        return {.error_count = error_count_};
    }

  private:
    SymbolTable &table_;
    TypeInfo &type_info_;
    lexing::DiagnosticSink *sink_ = nullptr;
    int error_count_ = 0;

    [[nodiscard]] std::string remember(const Node &node,
                                       std::string inferred_type) {
        type_info_.set(node, inferred_type);
        return inferred_type;
    }

    void emit_error(int line, std::string message) {
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

    [[nodiscard]] std::string visit(const ClassNode &node) {
        table_.enterClassScope(node.getClassName());
        ScopeExit exit_scope(table_);

        const auto body_type = visit(node.getBodyNode());
        if (is_error_type(body_type)) {
            return remember(node, error_type());
        }
        return remember(node, "void");
    }

    [[nodiscard]] std::string visit(const MainClassNode &node) {
        table_.enterClassScope(node.getMainClassName());
        ScopeExit exit_class_scope(table_);

        table_.enterMethodScope("main");
        ScopeExit exit_method_scope(table_);

        const auto body_type = visit(node.getBodyNode());
        if (is_error_type(body_type)) {
            return remember(node, error_type());
        }
        return remember(node, "void");
    }

    [[nodiscard]] std::string visit(const MethodNode &node) {
        table_.enterMethodScope(node.getMethodName());
        ScopeExit exit_scope(table_);

        const auto params_type = visit(node.getParametersNode());
        const auto body_return_type = visit(node.getBodyNode());

        bool valid = true;
        if (is_error_type(params_type) || is_error_type(body_return_type)) {
            valid = false;
        }

        const auto &signature_return_type = node.getMethodType();
        if (valid && signature_return_type != body_return_type) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Return type '" + signature_return_type +
                           "' in method '" + node.getMethodName() +
                           "' does not match returned type '" +
                           body_return_type + "'.\n");
            valid = false;
        }

        if (!valid) {
            return remember(node, error_type());
        }
        return remember(node, signature_return_type);
    }

    [[nodiscard]] std::string visit(const MethodWithoutParametersNode &node) {
        table_.enterMethodScope(node.getMethodName());
        ScopeExit exit_scope(table_);

        const auto body_return_type = visit(node.getBodyNode());
        bool valid = !is_error_type(body_return_type);

        const auto &signature_return_type = node.getMethodType();
        if (valid && signature_return_type != body_return_type) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Return type '" + signature_return_type +
                           "' in method '" + node.getMethodName() +
                           "' does not match returned type '" +
                           body_return_type + "'.\n");
            valid = false;
        }

        if (!valid) {
            return remember(node, error_type());
        }
        return remember(node, signature_return_type);
    }

    [[nodiscard]] std::string visit(const MethodParameterNode &node) {
        return remember(node, node.getParameterType());
    }

    [[nodiscard]] std::string visit(const VariableNode &node) {
        const auto &variable_type = node.getVariableType();
        if (!is_builtin_type(variable_type) &&
            table_.lookupClass(variable_type) == nullptr) {
            emit_error(node.lineno, "Error: (line " +
                                        std::to_string(node.lineno) +
                                        ") Unknown type '" + variable_type +
                                        "' for identifier '" +
                                        node.getVariableName() + "'.\n");
            return remember(node, error_type());
        }

        return remember(node, variable_type);
    }

    [[nodiscard]] std::string visit_type_node(const TypeNode &node) {
        return remember(node, static_cast<const Node &>(node).value);
    }

    [[nodiscard]] std::string visit_integer_node(const IntegerNode &node) {
        return remember(node, "int");
    }

    [[nodiscard]] std::string visit_boolean_node(const Node &node) {
        return remember(node, "boolean");
    }

    [[nodiscard]] std::string
    visit_identifier_node(const IdentifierNode &node) {
        const auto &identifier = static_cast<const Node &>(node).value;
        if (auto *variable = table_.lookupVariable(identifier);
            variable != nullptr) {
            return remember(node, variable->getType());
        }
        if (auto *klass = table_.lookupClass(identifier); klass != nullptr) {
            return remember(node, klass->getType());
        }
        if (auto *method = table_.lookupMethod(identifier); method != nullptr) {
            return remember(node, method->getType());
        }

        emit_error(node.lineno, "Error: (line " + std::to_string(node.lineno) +
                                    ") Undeclared identifier " + identifier +
                                    ".\n");
        return remember(node, error_type());
    }

    [[nodiscard]] std::string visit_this_node(const ThisNode &node) {
        auto *lookup = table_.lookupVariable("this");
        if (lookup == nullptr) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Undeclared identifier (type this)this.\n");
            return remember(node, error_type());
        }

        return remember(node, lookup->getType());
    }

    [[nodiscard]] std::string visit_arithmetic_expression(const Node &node) {
        const auto *lhs = child_at(node, 0);
        const auto *rhs = child_at(node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            return remember(node, error_type());
        }

        const auto lhs_type = visit(*lhs);
        const auto rhs_type = visit(*rhs);

        if (!is_error_type(lhs_type) && !is_error_type(rhs_type) &&
            lhs_type == "int" && rhs_type == "int") {
            return remember(node, "int");
        }

        if (!is_error_type(lhs_type) && !is_error_type(rhs_type)) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) + ") " +
                           node.type +
                           " operation does not support operands of types '" +
                           lhs_type + "' and '" + rhs_type + "'.\n");
        }

        return remember(node, error_type());
    }

    [[nodiscard]] std::string visit_boolean_expression(const Node &node) {
        const auto *lhs = child_at(node, 0);
        const auto *rhs = child_at(node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            return remember(node, error_type());
        }

        const auto lhs_type = visit(*lhs);
        const auto rhs_type = visit(*rhs);

        if (!is_error_type(lhs_type) && !is_error_type(rhs_type) &&
            lhs_type == "boolean" && rhs_type == "boolean") {
            return remember(node, "boolean");
        }

        if (!is_error_type(lhs_type) && !is_error_type(rhs_type)) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) + ") " +
                           node.type +
                           " operation does not support operands of types " +
                           lhs_type + " and " + rhs_type + ".\n");
        }

        return remember(node, error_type());
    }

    [[nodiscard]] std::string visit_logical_expression(const Node &node) {
        const auto *lhs = child_at(node, 0);
        const auto *rhs = child_at(node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            return remember(node, error_type());
        }

        const auto lhs_type = visit(*lhs);
        const auto rhs_type = visit(*rhs);

        if (!is_error_type(lhs_type) && !is_error_type(rhs_type) &&
            lhs_type == "int" && rhs_type == "int") {
            return remember(node, "boolean");
        }

        if (!is_error_type(lhs_type) && !is_error_type(rhs_type)) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) + ") " +
                           node.type +
                           " operation does not support operands of types '" +
                           lhs_type + "' and '" + rhs_type + "'.\n");
        }

        return remember(node, error_type());
    }

    [[nodiscard]] std::string
    visit_equal_to_expression(const EqualToNode &node) {
        const auto *lhs = child_at(node, 0);
        const auto *rhs = child_at(node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            return remember(node, error_type());
        }

        const auto lhs_type = visit(*lhs);
        const auto rhs_type = visit(*rhs);

        const bool same_boolean =
            lhs_type == "boolean" && rhs_type == "boolean";
        const bool same_integer = lhs_type == "int" && rhs_type == "int";
        if (!is_error_type(lhs_type) && !is_error_type(rhs_type) &&
            (same_boolean || same_integer)) {
            return remember(node, "boolean");
        }

        if (!is_error_type(lhs_type) && !is_error_type(rhs_type)) {
            emit_error(
                node.lineno,
                "Error: (line " + std::to_string(node.lineno) +
                    ") Operator '==' does not support operands of types '" +
                    lhs_type + "' and '" + rhs_type + "'.\n");
        }

        return remember(node, error_type());
    }

    [[nodiscard]] std::string visit_not_expression(const NotNode &node) {
        const auto *expr = child_at(node, 0);
        if (expr == nullptr) {
            return remember(node, error_type());
        }

        const auto expr_type = visit(*expr);
        if (!is_error_type(expr_type) && expr_type == "boolean") {
            return remember(node, "boolean");
        }

        if (!is_error_type(expr_type)) {
            emit_error(
                node.lineno,
                "Error: (line " + std::to_string(node.lineno) +
                    ") Invalid type '" + expr_type +
                    "' for negation operator, expected type 'boolean'.\n");
        }

        return remember(node, error_type());
    }

    [[nodiscard]] std::string visit_array_access(const ArrayAccessNode &node) {
        const auto *array = child_at(node, 0);
        const auto *index = child_at(node, 1);
        if (array == nullptr || index == nullptr) {
            return remember(node, error_type());
        }

        const auto array_type = visit(*array);
        const auto index_type = visit(*index);

        if (is_error_type(index_type) || is_error_type(array_type)) {
            return remember(node, error_type());
        }

        if (index_type != "int") {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Invalid array index type '" + index_type +
                           "', expected type 'int'.\n");
            return remember(node, error_type());
        }

        if (array_type != "int[]") {
            emit_error(node.lineno, "Error: (line " +
                                        std::to_string(node.lineno) +
                                        ") Invalid array type '" + array_type +
                                        "', expected type 'int[]'.\n");
            return remember(node, error_type());
        }

        return remember(node, "int");
    }

    [[nodiscard]] std::string visit_array_length(const ArrayLengthNode &node) {
        const auto array_type = visit(node.getArrayNode());
        if (!is_error_type(array_type) && array_type == "int[]") {
            return remember(node, "int");
        }

        if (!is_error_type(array_type)) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Invalid type '" + array_type +
                           "' for array length, expected type 'int[]'.\n");
        }

        return remember(node, error_type());
    }

    [[nodiscard]] std::string
    visit_integer_array_allocation(const IntegerArrayAllocationNode &node) {
        const auto length_type = visit(node.getLengthNode());
        if (!is_error_type(length_type) && length_type == "int") {
            return remember(node, "int[]");
        }

        if (!is_error_type(length_type)) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Invalid type '" + length_type +
                           "' for array length, expected type 'int'.\n");
        }

        return remember(node, error_type());
    }

    [[nodiscard]] std::string
    visit_class_allocation(const ClassAllocationNode &node) {
        if (table_.lookupClass(node.value) == nullptr) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Unknown class '" + node.value + "'.\n");
            return remember(node, error_type());
        }

        return remember(node, node.value);
    }

    [[nodiscard]] std::string visit_method_call(const MethodCallNode &node) {
        const auto *object = child_at(node, 0);
        const auto *method_identifier = child_at(node, 1);
        const auto *expr_list = child_at(node, 2);
        if (object == nullptr || method_identifier == nullptr ||
            expr_list == nullptr) {
            return remember(node, error_type());
        }

        const auto caller_type = visit(*object);
        if (is_error_type(caller_type)) {
            return remember(node, error_type());
        }

        auto *calling_class = table_.lookupClass(caller_type);
        const auto &method_name = method_identifier->value;
        if (calling_class == nullptr) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Method '" + method_name +
                           "' not declared for class '" + caller_type + "'.\n");
            return remember(node, error_type());
        }

        auto *method = calling_class->lookupMethod(method_name);
        if (method == nullptr) {
            emit_error(node.lineno, "Error: (line " +
                                        std::to_string(node.lineno) +
                                        ") Method '" + method_name +
                                        "' not declared for class '" +
                                        calling_class->getID() + "'.\n");
            return remember(node, error_type());
        }

        bool valid = true;
        std::vector<std::string> argument_types;
        argument_types.reserve(expr_list->children.size());
        for (const auto &arg : expr_list->children) {
            argument_types.push_back(visit(*arg));
        }

        const auto expected_arguments = method->getParameterCount();
        const auto passed_arguments = argument_types.size();
        if (expected_arguments != passed_arguments) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Method '" + method->getID() + "' expects " +
                           std::to_string(expected_arguments) + " arguments, " +
                           std::to_string(passed_arguments) +
                           " arguments given.\n");
            valid = false;
        }

        const auto &params = method->getParameters();
        const auto comparable_count =
            std::min(params.size(), argument_types.size());
        for (std::size_t i = 0; i < comparable_count; ++i) {
            const auto &arg_type = argument_types[i];
            if (is_error_type(arg_type)) {
                valid = false;
                continue;
            }

            const auto &param_type = params[i]->getType();
            if (param_type != arg_type) {
                const auto arg_number = i + 1;
                emit_error(node.lineno,
                           "Error: (line " + std::to_string(node.lineno) +
                               ") Argument " + std::to_string(arg_number) +
                               " of type '" + arg_type +
                               "' does not match parameter " +
                               std::to_string(arg_number) + " of type '" +
                               param_type + "'.\n");
                valid = false;
            }
        }

        if (!valid) {
            return remember(node, error_type());
        }
        return remember(node, method->getType());
    }

    [[nodiscard]] std::string visit_method_call_without_arguments(
        const MethodCallWithoutArgumentsNode &node) {
        const auto *object = child_at(node, 0);
        const auto *method_identifier = child_at(node, 1);
        if (object == nullptr || method_identifier == nullptr) {
            return remember(node, error_type());
        }

        const auto caller_type = visit(*object);
        if (is_error_type(caller_type)) {
            return remember(node, error_type());
        }

        auto *calling_class = table_.lookupClass(caller_type);
        const auto &method_name = method_identifier->value;
        if (calling_class == nullptr) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Method '" + method_name +
                           "' not declared for class '" + caller_type + "'.\n");
            return remember(node, error_type());
        }

        auto *method = calling_class->lookupMethod(method_name);
        if (method == nullptr) {
            emit_error(node.lineno, "Error: (line " +
                                        std::to_string(node.lineno) +
                                        ") Method '" + method_name +
                                        "' not declared for class '" +
                                        calling_class->getID() + "'.\n");
            return remember(node, error_type());
        }

        const auto expected_arguments = method->getParameterCount();
        if (expected_arguments != 0) {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Method '" + method->getID() + "' expects " +
                           std::to_string(expected_arguments) +
                           " arguments, no arguments passed.\n");
            return remember(node, error_type());
        }

        return remember(node, method->getType());
    }

    [[nodiscard]] std::string visit_assign_statement(const AssignNode &node) {
        const auto *lhs = child_at(node, 0);
        const auto *rhs = child_at(node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            return remember(node, error_type());
        }

        const auto lhs_type = visit(*lhs);
        const auto rhs_type = visit(*rhs);

        bool valid = true;
        if (is_error_type(lhs_type) || is_error_type(rhs_type)) {
            valid = false;
        } else if (lhs_type != rhs_type) {
            emit_error(node.lineno, "Error: (line " +
                                        std::to_string(node.lineno) +
                                        ") Cannot assign type '" + rhs_type +
                                        "' to type '" + lhs_type + "'.\n");
            valid = false;
        }

        if (!valid) {
            return remember(node, error_type());
        }
        return remember(node, "void");
    }

    [[nodiscard]] std::string
    visit_array_assign_statement(const ArrayAssignNode &node) {
        const auto *lhs = child_at(node, 0);
        const auto *index = child_at(node, 1);
        if (lhs == nullptr || index == nullptr) {
            return remember(node, error_type());
        }

        const auto index_type = visit(*index);
        const auto lhs_type = visit(*lhs);
        const auto rhs_type = visit(node.getRightExprNode());

        bool valid = true;
        if (!is_error_type(index_type) && index_type != "int") {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Invalid array index type '" + index_type +
                           "', expected type 'int'.\n");
            valid = false;
        } else if (is_error_type(index_type)) {
            valid = false;
        }

        if (!is_error_type(lhs_type) && lhs_type != "int[]") {
            emit_error(node.lineno, "Error: (line " +
                                        std::to_string(node.lineno) +
                                        ") Invalid array type '" + lhs_type +
                                        "', expected type 'int[]'.\n");
            valid = false;
        } else if (is_error_type(lhs_type)) {
            valid = false;
        }

        if (!is_error_type(rhs_type) && rhs_type != "int") {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Cannot assign value of type '" + rhs_type +
                           "', to array of type '" + lhs_type + "'.\n");
            valid = false;
        } else if (is_error_type(rhs_type)) {
            valid = false;
        }

        if (!valid) {
            return remember(node, error_type());
        }
        return remember(node, "void");
    }

    [[nodiscard]] std::string visit_print_statement(const PrintNode &node) {
        const auto *expr = child_at(node, 0);
        if (expr == nullptr) {
            return remember(node, error_type());
        }

        const auto expr_type = visit(*expr);
        if (is_error_type(expr_type)) {
            return remember(node, error_type());
        }
        return remember(node, "void");
    }

    [[nodiscard]] std::string
    visit_control_statement(const ControlStatementNode &node) {
        const auto *cond = child_at(node, 0);
        if (cond == nullptr) {
            return remember(node, error_type());
        }

        bool valid = true;
        const auto cond_type = visit(*cond);
        if (is_error_type(cond_type)) {
            valid = false;
        } else if (cond_type != "boolean") {
            emit_error(node.lineno,
                       "Error: (line " + std::to_string(node.lineno) +
                           ") Condition for " + node.type +
                           "-statement of invalid type " + cond_type + ".\n");
            valid = false;
        }

        std::size_t index = 1;
        while (const auto *statement = child_at(node, index)) {
            const auto stmt_type = visit(*statement);
            if (is_error_type(stmt_type)) {
                valid = false;
            }
            index += 1;
        }

        if (!valid) {
            return remember(node, error_type());
        }
        return remember(node, "void");
    }

    [[nodiscard]] std::string visit_method_body(const MethodBodyNode &node) {
        const auto *body = child_at(node, 0);
        const auto *return_value = child_at(node, 1);
        if (body == nullptr || return_value == nullptr) {
            return remember(node, error_type());
        }

        const auto body_type = visit(*body);
        const auto return_type = visit(*return_value);
        if (is_error_type(body_type) || is_error_type(return_type)) {
            return remember(node, error_type());
        }

        return remember(node, return_type);
    }

    [[nodiscard]] std::string
    visit_return_only_method_body(const ReturnOnlyMethodBodyNode &node) {
        const auto *return_value = child_at(node, 0);
        if (return_value == nullptr) {
            return remember(node, error_type());
        }

        const auto return_type = visit(*return_value);
        if (is_error_type(return_type)) {
            return remember(node, error_type());
        }

        return remember(node, return_type);
    }

    [[nodiscard]] std::string visit_generic_node(const Node &node) {
        bool valid = true;
        for (const auto &child : node.children) {
            const auto child_type = visit(*child);
            if (is_error_type(child_type)) {
                valid = false;
            }
        }

        if (!valid) {
            return remember(node, error_type());
        }
        return remember(node, "void");
    }

    [[nodiscard]] std::string visit(const Node &node) {
        if (const auto *class_node = dynamic_cast<const ClassNode *>(&node)) {
            return visit(*class_node);
        }
        if (const auto *main_class_node =
                dynamic_cast<const MainClassNode *>(&node)) {
            return visit(*main_class_node);
        }
        if (const auto *method_node = dynamic_cast<const MethodNode *>(&node)) {
            return visit(*method_node);
        }
        if (const auto *method_without_parameters_node =
                dynamic_cast<const MethodWithoutParametersNode *>(&node)) {
            return visit(*method_without_parameters_node);
        }
        if (const auto *method_parameter_node =
                dynamic_cast<const MethodParameterNode *>(&node)) {
            return visit(*method_parameter_node);
        }
        if (const auto *variable_node =
                dynamic_cast<const VariableNode *>(&node)) {
            return visit(*variable_node);
        }

        if (const auto *type_node = dynamic_cast<const TypeNode *>(&node)) {
            return visit_type_node(*type_node);
        }
        if (const auto *integer_node =
                dynamic_cast<const IntegerNode *>(&node)) {
            return visit_integer_node(*integer_node);
        }
        if (dynamic_cast<const TrueNode *>(&node) != nullptr ||
            dynamic_cast<const FalseNode *>(&node) != nullptr) {
            return visit_boolean_node(node);
        }
        if (const auto *identifier_node =
                dynamic_cast<const IdentifierNode *>(&node)) {
            return visit_identifier_node(*identifier_node);
        }
        if (const auto *this_node = dynamic_cast<const ThisNode *>(&node)) {
            return visit_this_node(*this_node);
        }

        if (dynamic_cast<const ArithmeticExpressionNode *>(&node) != nullptr) {
            return visit_arithmetic_expression(node);
        }
        if (dynamic_cast<const BooleanExpressionNode *>(&node) != nullptr) {
            return visit_boolean_expression(node);
        }
        if (dynamic_cast<const LogicalExpressionNode *>(&node) != nullptr) {
            return visit_logical_expression(node);
        }
        if (const auto *equal_to_node =
                dynamic_cast<const EqualToNode *>(&node)) {
            return visit_equal_to_expression(*equal_to_node);
        }
        if (const auto *not_node = dynamic_cast<const NotNode *>(&node)) {
            return visit_not_expression(*not_node);
        }

        if (const auto *array_access_node =
                dynamic_cast<const ArrayAccessNode *>(&node)) {
            return visit_array_access(*array_access_node);
        }
        if (const auto *array_length_node =
                dynamic_cast<const ArrayLengthNode *>(&node)) {
            return visit_array_length(*array_length_node);
        }
        if (const auto *integer_array_allocation_node =
                dynamic_cast<const IntegerArrayAllocationNode *>(&node)) {
            return visit_integer_array_allocation(
                *integer_array_allocation_node);
        }
        if (const auto *class_allocation_node =
                dynamic_cast<const ClassAllocationNode *>(&node)) {
            return visit_class_allocation(*class_allocation_node);
        }

        if (const auto *method_call_node =
                dynamic_cast<const MethodCallNode *>(&node)) {
            return visit_method_call(*method_call_node);
        }
        if (const auto *method_call_without_arguments_node =
                dynamic_cast<const MethodCallWithoutArgumentsNode *>(&node)) {
            return visit_method_call_without_arguments(
                *method_call_without_arguments_node);
        }

        if (const auto *assign_node = dynamic_cast<const AssignNode *>(&node)) {
            return visit_assign_statement(*assign_node);
        }
        if (const auto *array_assign_node =
                dynamic_cast<const ArrayAssignNode *>(&node)) {
            return visit_array_assign_statement(*array_assign_node);
        }
        if (const auto *print_node = dynamic_cast<const PrintNode *>(&node)) {
            return visit_print_statement(*print_node);
        }
        if (const auto *control_statement_node =
                dynamic_cast<const ControlStatementNode *>(&node)) {
            return visit_control_statement(*control_statement_node);
        }

        if (const auto *method_body_node =
                dynamic_cast<const MethodBodyNode *>(&node)) {
            return visit_method_body(*method_body_node);
        }
        if (const auto *return_only_method_body_node =
                dynamic_cast<const ReturnOnlyMethodBodyNode *>(&node)) {
            return visit_return_only_method_body(*return_only_method_body_node);
        }

        return visit_generic_node(node);
    }
};

} // namespace

void TypeInfo::set(const Node &node, std::string type_name) {
    map_[&node] = std::move(type_name);
}

const std::string *TypeInfo::get(const Node &node) const {
    if (const auto it = map_.find(&node); it != map_.end()) {
        return &it->second;
    }
    return nullptr;
}

TypeCheckResult check_types(const Node &root, SymbolTable &table,
                            TypeInfo *type_info, lexing::DiagnosticSink *sink) {
    assert(type_info != nullptr);
    if (type_info == nullptr) {
        return {.error_count = 1};
    }
    *type_info = TypeInfo{};

    while (table.getParentScope() != nullptr) {
        table.exitScope();
    }

    TypeCheckVisitor visitor(table, *type_info, sink);
    return visitor.run(root);
}
