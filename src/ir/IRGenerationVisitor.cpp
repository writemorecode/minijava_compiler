#include "ir/IRGenerationVisitor.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <utility>

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
#include "ast/MethodWithoutParametersNode.hpp"
#include "ast/Node.h"
#include "ast/NotNode.hpp"
#include "ast/StatementNode.hpp"
#include "ast/ThisNode.hpp"
#include "ir/ArithmeticTac.hpp"
#include "ir/CFG.hpp"
#include "ir/LogicalTac.hpp"
#include "ir/Tac.hpp"
#include "semantic/Class.hpp"
#include "semantic/Method.hpp"
#include "semantic/SymbolTable.hpp"

namespace {

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

} // namespace

void IRGenerationVisitor::emit_error(int line, std::string message) {
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

Operand IRGenerationVisitor::eval(const Node &node) {
    node.accept(*this);

    if (const auto it = values_.find(&node); it != values_.end()) {
        return it->second;
    }

    return std::string{};
}

void IRGenerationVisitor::set_value(const Node &node, Operand value) {
    values_[&node] = std::move(value);
}

void IRGenerationVisitor::visit_generic(const Node &node) {
    for (const auto &child : node.children) {
        (void)eval(*child);
    }

    set_value(node, std::string{});
}

void IRGenerationVisitor::visit(const ClassNode &node) {
    auto *current_class = table_.lookupClass(node.getClassName());
    if (current_class == nullptr) {
        emit_error(node.lineno, "Error: (line " + std::to_string(node.lineno) +
                                    ") IR generation could not find class '" +
                                    node.getClassName() + "'.\n");
        set_value(node, std::string{});
        return;
    }

    table_.enterClassScope(current_class);
    ScopeExit exit_scope(table_);

    (void)eval(node.getBodyNode());
    set_value(node, std::string{});
}

void IRGenerationVisitor::visit(const MainClassNode &node) {
    table_.enterClassScope(node.getMainClassName());
    ScopeExit exit_class_scope(table_);

    table_.enterMethodScope("main");
    ScopeExit exit_method_scope(table_);

    graph_.setCurrentBlock(
        graph_.addMethodRootBlock(node.getMainClassName(), "main"));

    (void)eval(node.getBodyNode());

    auto *current_block = graph_.getCurrentBlock();
    set_value(node, current_block != nullptr ? Operand{current_block->getName()}
                                             : Operand{std::string{}});
}

void IRGenerationVisitor::visit(const MethodNode &node) {
    auto *current_class = dynamic_cast<Class *>(table_.getCurrentRecord());
    if (current_class == nullptr) {
        emit_error(node.lineno,
                   "Error: (line " + std::to_string(node.lineno) +
                       ") IR generation expected class scope for method '" +
                       node.getMethodName() + "'.\n");
        set_value(node, std::string{});
        return;
    }

    auto *current_method = table_.lookupMethod(node.getMethodName());
    if (current_method == nullptr) {
        emit_error(node.lineno, "Error: (line " + std::to_string(node.lineno) +
                                    ") IR generation could not find method '" +
                                    node.getMethodName() + "'.\n");
        set_value(node, std::string{});
        return;
    }

    table_.enterMethodScope(current_method);
    ScopeExit exit_scope(table_);

    graph_.setCurrentBlock(graph_.addMethodRootBlock(current_class->getID(),
                                                     node.getMethodName()));

    (void)eval(node.getBodyNode());

    auto *current_block = graph_.getCurrentBlock();
    set_value(node, current_block != nullptr ? Operand{current_block->getName()}
                                             : Operand{std::string{}});
}

void IRGenerationVisitor::visit(const MethodWithoutParametersNode &node) {
    auto *current_class = dynamic_cast<Class *>(table_.getCurrentRecord());
    if (current_class == nullptr) {
        emit_error(node.lineno,
                   "Error: (line " + std::to_string(node.lineno) +
                       ") IR generation expected class scope for method '" +
                       node.getMethodName() + "'.\n");
        set_value(node, std::string{});
        return;
    }

    auto *current_method = table_.lookupMethod(node.getMethodName());
    if (current_method == nullptr) {
        emit_error(node.lineno, "Error: (line " + std::to_string(node.lineno) +
                                    ") IR generation could not find method '" +
                                    node.getMethodName() + "'.\n");
        set_value(node, std::string{});
        return;
    }

    table_.enterMethodScope(current_method);
    ScopeExit exit_scope(table_);

    graph_.setCurrentBlock(graph_.addMethodRootBlock(current_class->getID(),
                                                     node.getMethodName()));

    (void)eval(node.getBodyNode());

    auto *current_block = graph_.getCurrentBlock();
    set_value(node, current_block != nullptr ? Operand{current_block->getName()}
                                             : Operand{std::string{}});
}

void IRGenerationVisitor::visit(const Node &node) {
    if (const auto *class_node = dynamic_cast<const ClassNode *>(&node)) {
        visit(*class_node);
        return;
    }
    if (const auto *main_class_node =
            dynamic_cast<const MainClassNode *>(&node)) {
        visit(*main_class_node);
        return;
    }
    if (const auto *method_node = dynamic_cast<const MethodNode *>(&node)) {
        visit(*method_node);
        return;
    }
    if (const auto *method_without_parameters_node =
            dynamic_cast<const MethodWithoutParametersNode *>(&node)) {
        visit(*method_without_parameters_node);
        return;
    }

    if (const auto *plus_node = dynamic_cast<const PlusNode *>(&node)) {
        const auto *lhs = child_at(*plus_node, 0);
        const auto *rhs = child_at(*plus_node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto lhs_name = eval(*lhs);
        const auto rhs_name = eval(*rhs);
        const auto name = graph_.getTemporaryName();
        table_.addIntegerVariable(name);
        graph_.addInstruction(new AddTac(name, lhs_name, rhs_name));
        set_value(node, name);
        return;
    }

    if (const auto *minus_node = dynamic_cast<const MinusNode *>(&node)) {
        const auto *lhs = child_at(*minus_node, 0);
        const auto *rhs = child_at(*minus_node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto lhs_name = eval(*lhs);
        const auto rhs_name = eval(*rhs);
        const auto name = graph_.getTemporaryName();
        table_.addIntegerVariable(name);
        graph_.addInstruction(new SubtractTac(name, lhs_name, rhs_name));
        set_value(node, name);
        return;
    }

    if (const auto *multiplication_node =
            dynamic_cast<const MultiplicationNode *>(&node)) {
        const auto *lhs = child_at(*multiplication_node, 0);
        const auto *rhs = child_at(*multiplication_node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto lhs_name = eval(*lhs);
        const auto rhs_name = eval(*rhs);
        const auto name = graph_.getTemporaryName();
        table_.addIntegerVariable(name);
        graph_.addInstruction(new MultiplyTac(name, lhs_name, rhs_name));
        set_value(node, name);
        return;
    }

    if (const auto *division_node = dynamic_cast<const DivisionNode *>(&node)) {
        const auto *lhs = child_at(*division_node, 0);
        const auto *rhs = child_at(*division_node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto lhs_name = eval(*lhs);
        const auto rhs_name = eval(*rhs);
        const auto name = graph_.getTemporaryName();
        table_.addIntegerVariable(name);
        graph_.addInstruction(new DivideTac(name, lhs_name, rhs_name));
        set_value(node, name);
        return;
    }

    if (const auto *less_than_node =
            dynamic_cast<const LessThanNode *>(&node)) {
        const auto *lhs = child_at(*less_than_node, 0);
        const auto *rhs = child_at(*less_than_node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto lhs_name = eval(*lhs);
        const auto rhs_name = eval(*rhs);
        const auto name = graph_.getTemporaryName();
        table_.addBooleanVariable(name);
        graph_.addInstruction(new LessThanTac(name, lhs_name, rhs_name));
        set_value(node, name);
        return;
    }

    if (const auto *greater_than_node =
            dynamic_cast<const GreaterThanNode *>(&node)) {
        const auto *lhs = child_at(*greater_than_node, 0);
        const auto *rhs = child_at(*greater_than_node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto lhs_name = eval(*lhs);
        const auto rhs_name = eval(*rhs);
        const auto name = graph_.getTemporaryName();
        table_.addBooleanVariable(name);
        graph_.addInstruction(new GreaterThanTac(name, lhs_name, rhs_name));
        set_value(node, name);
        return;
    }

    if (const auto *equal_to_node = dynamic_cast<const EqualToNode *>(&node)) {
        const auto *lhs = child_at(*equal_to_node, 0);
        const auto *rhs = child_at(*equal_to_node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto lhs_name = eval(*lhs);
        const auto rhs_name = eval(*rhs);
        const auto name = graph_.getTemporaryName();
        table_.addBooleanVariable(name);
        graph_.addInstruction(new EqualToTac(name, lhs_name, rhs_name));
        set_value(node, name);
        return;
    }

    if (const auto *not_node = dynamic_cast<const NotNode *>(&node)) {
        const auto *rhs = child_at(*not_node, 0);
        if (rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto rhs_name = eval(*rhs);
        const auto name = graph_.getTemporaryName();
        table_.addBooleanVariable(name);
        graph_.addInstruction(new NotTac(name, rhs_name));
        set_value(node, name);
        return;
    }

    if (const auto *and_node = dynamic_cast<const AndNode *>(&node)) {
        const auto *lhs = child_at(*and_node, 0);
        const auto *rhs = child_at(*and_node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto lhs_name = eval(*lhs);
        const auto name = graph_.getTemporaryName();
        table_.addBooleanVariable(name);

        auto *rhs_eval_block = graph_.newBlock();
        auto *true_block = graph_.newBlock();
        auto *false_block = graph_.newBlock();
        auto *join_block = graph_.newBlock();

        auto *entry_block = graph_.getCurrentBlock();
        graph_.addInstruction(
            new CondJumpTac(false_block->getName(), lhs_name));
        graph_.addInstruction(new JumpTac(rhs_eval_block->getName()));
        entry_block->setTrueBlock(rhs_eval_block);
        entry_block->setFalseBlock(false_block);

        graph_.setCurrentBlock(rhs_eval_block);
        const auto rhs_name = eval(*rhs);
        graph_.addInstruction(
            new CondJumpTac(false_block->getName(), rhs_name));
        graph_.addInstruction(new JumpTac(true_block->getName()));
        rhs_eval_block->setTrueBlock(true_block);
        rhs_eval_block->setFalseBlock(false_block);

        graph_.setCurrentBlock(true_block);
        graph_.addInstruction(new CopyTac(1, name));
        graph_.addInstruction(new JumpTac(join_block->getName()));
        true_block->setTrueBlock(join_block);

        graph_.setCurrentBlock(false_block);
        graph_.addInstruction(new CopyTac(0, name));
        graph_.addInstruction(new JumpTac(join_block->getName()));
        false_block->setTrueBlock(join_block);

        graph_.setCurrentBlock(join_block);
        set_value(node, name);
        return;
    }

    if (const auto *or_node = dynamic_cast<const OrNode *>(&node)) {
        const auto *lhs = child_at(*or_node, 0);
        const auto *rhs = child_at(*or_node, 1);
        if (lhs == nullptr || rhs == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto lhs_name = eval(*lhs);
        const auto name = graph_.getTemporaryName();
        table_.addBooleanVariable(name);

        auto *rhs_eval_block = graph_.newBlock();
        auto *true_block = graph_.newBlock();
        auto *false_block = graph_.newBlock();
        auto *join_block = graph_.newBlock();

        auto *entry_block = graph_.getCurrentBlock();
        graph_.addInstruction(
            new CondJumpTac(rhs_eval_block->getName(), lhs_name));
        graph_.addInstruction(new JumpTac(true_block->getName()));
        entry_block->setTrueBlock(true_block);
        entry_block->setFalseBlock(rhs_eval_block);

        graph_.setCurrentBlock(rhs_eval_block);
        const auto rhs_name = eval(*rhs);
        graph_.addInstruction(
            new CondJumpTac(false_block->getName(), rhs_name));
        graph_.addInstruction(new JumpTac(true_block->getName()));
        rhs_eval_block->setTrueBlock(true_block);
        rhs_eval_block->setFalseBlock(false_block);

        graph_.setCurrentBlock(true_block);
        graph_.addInstruction(new CopyTac(1, name));
        graph_.addInstruction(new JumpTac(join_block->getName()));
        true_block->setTrueBlock(join_block);

        graph_.setCurrentBlock(false_block);
        graph_.addInstruction(new CopyTac(0, name));
        graph_.addInstruction(new JumpTac(join_block->getName()));
        false_block->setTrueBlock(join_block);

        graph_.setCurrentBlock(join_block);
        set_value(node, name);
        return;
    }

    if (const auto *identifier_node =
            dynamic_cast<const IdentifierNode *>(&node)) {
        set_value(*identifier_node,
                  static_cast<const Node &>(*identifier_node).value);
        return;
    }

    if (const auto *integer_node = dynamic_cast<const IntegerNode *>(&node)) {
        set_value(*integer_node,
                  std::stoi(static_cast<const Node &>(*integer_node).value));
        return;
    }

    if (dynamic_cast<const TrueNode *>(&node) != nullptr) {
        set_value(node, 1);
        return;
    }

    if (dynamic_cast<const FalseNode *>(&node) != nullptr) {
        set_value(node, 0);
        return;
    }

    if (dynamic_cast<const ThisNode *>(&node) != nullptr) {
        set_value(node, std::string{"this"});
        return;
    }

    if (const auto *class_allocation_node =
            dynamic_cast<const ClassAllocationNode *>(&node)) {
        const auto name = graph_.getTemporaryName();
        table_.addVariable(class_allocation_node->value, name);
        graph_.addInstruction(new NewTac(name, class_allocation_node->value));
        set_value(node, name);
        return;
    }

    if (const auto *array_allocation_node =
            dynamic_cast<const IntegerArrayAllocationNode *>(&node)) {
        const auto name = graph_.getTemporaryName();
        table_.addVariable("int[]", name);
        const auto length_name = eval(array_allocation_node->getLengthNode());
        graph_.addInstruction(new NewArrayTac(name, length_name));
        set_value(node, name);
        return;
    }

    if (const auto *array_access_node =
            dynamic_cast<const ArrayAccessNode *>(&node)) {
        const auto *array = child_at(*array_access_node, 0);
        const auto *index = child_at(*array_access_node, 1);
        if (array == nullptr || index == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto array_name = eval(*array);
        const auto index_name = eval(*index);
        const auto name = graph_.getTemporaryName();
        table_.addIntegerVariable(name);
        graph_.addInstruction(new ArrayAccessTac(name, array_name, index_name));
        set_value(node, name);
        return;
    }

    if (const auto *array_length_node =
            dynamic_cast<const ArrayLengthNode *>(&node)) {
        const auto name = graph_.getTemporaryName();
        table_.addIntegerVariable(name);
        const auto array_name = eval(array_length_node->getArrayNode());
        graph_.addInstruction(new ArrayLengthTac(name, array_name));
        set_value(node, name);
        return;
    }

    if (const auto *method_call_node =
            dynamic_cast<const MethodCallNode *>(&node)) {
        const auto *object = child_at(*method_call_node, 0);
        const auto *identifier = child_at(*method_call_node, 1);
        const auto *expr_list = child_at(*method_call_node, 2);
        if (object == nullptr || identifier == nullptr ||
            expr_list == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto *caller_type = graph_.typeOf(*object);
        assert(caller_type != nullptr &&
               "Type information missing for method call receiver");
        if (caller_type == nullptr || caller_type->empty()) {
            set_value(node, std::string{});
            return;
        }

        auto *calling_class = table_.lookupClass(*caller_type);
        if (calling_class == nullptr) {
            set_value(node, std::string{});
            return;
        }

        auto const *method = calling_class->lookupMethod(identifier->value);
        if (method == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto &method_type = method->getType();
        const auto receiver = eval(*object);

        for (const auto &arg : expr_list->children) {
            const auto arg_name = eval(*arg);
            graph_.addInstruction(new ParamTac(arg_name));
        }

        const auto name = graph_.getTemporaryName();
        table_.addVariable(method_type, name);

        const auto method_target = *caller_type + "." + identifier->value;
        const auto arg_count = static_cast<int>(expr_list->children.size());
        graph_.addInstruction(
            new MethodCallTac(name, receiver, method_target, arg_count));
        set_value(node, name);
        return;
    }

    if (const auto *method_call_without_arguments_node =
            dynamic_cast<const MethodCallWithoutArgumentsNode *>(&node)) {
        const auto *object = child_at(*method_call_without_arguments_node, 0);
        const auto *identifier =
            child_at(*method_call_without_arguments_node, 1);
        if (object == nullptr || identifier == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto *caller_type = graph_.typeOf(*object);
        assert(caller_type != nullptr &&
               "Type information missing for method call receiver");
        if (caller_type == nullptr || caller_type->empty()) {
            set_value(node, std::string{});
            return;
        }

        auto *calling_class = table_.lookupClass(*caller_type);
        if (calling_class == nullptr) {
            set_value(node, std::string{});
            return;
        }

        auto const *method = calling_class->lookupMethod(identifier->value);
        if (method == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto &method_type = method->getType();
        const auto receiver = eval(*object);

        const auto name = graph_.getTemporaryName();
        table_.addVariable(method_type, name);
        const auto method_target = *caller_type + "." + identifier->value;
        graph_.addInstruction(
            new MethodCallTac(name, receiver, method_target, 0));
        set_value(node, name);
        return;
    }

    if (const auto *assign_node = dynamic_cast<const AssignNode *>(&node)) {
        const auto *identifier = child_at(*assign_node, 0);
        const auto *expr = child_at(*assign_node, 1);
        if (identifier == nullptr || expr == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto rhs_name = eval(*expr);
        const auto &lhs_name = identifier->value;
        graph_.addInstruction(new CopyTac(rhs_name, lhs_name));
        set_value(node, lhs_name);
        return;
    }

    if (const auto *array_assign_node =
            dynamic_cast<const ArrayAssignNode *>(&node)) {
        const auto *identifier = child_at(*array_assign_node, 0);
        const auto *index_expr = child_at(*array_assign_node, 1);
        if (identifier == nullptr || index_expr == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto index_name = eval(*index_expr);
        const auto rhs_name = eval(array_assign_node->getRightExprNode());
        const auto &array_name = identifier->value;
        graph_.addInstruction(
            new ArrayCopyTac(array_name, index_name, rhs_name));
        set_value(node, array_name);
        return;
    }

    if (const auto *print_node = dynamic_cast<const PrintNode *>(&node)) {
        const auto *expr = child_at(*print_node, 0);
        if (expr == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto value = eval(*expr);
        graph_.addInstruction(new PrintTac(value));
        set_value(node, std::string{});
        return;
    }

    if (const auto *if_node = dynamic_cast<const IfNode *>(&node)) {
        const auto *cond = child_at(*if_node, 0);
        const auto *stmt = child_at(*if_node, 1);
        if (cond == nullptr || stmt == nullptr) {
            set_value(node, std::string{});
            return;
        }

        auto *true_block = graph_.newBlock();
        auto *join_block = graph_.newBlock();
        const auto &join_label = join_block->getName();
        const auto &true_label = true_block->getName();

        const auto cond_name = eval(*cond);
        graph_.addInstruction(new CondJumpTac(join_label, cond_name));
        graph_.addInstruction(new JumpTac(true_label));

        auto *current_block = graph_.getCurrentBlock();
        current_block->setTrueBlock(true_block);
        current_block->setFalseBlock(join_block);

        graph_.setCurrentBlock(true_block);
        (void)eval(*stmt);
        graph_.addInstruction(new JumpTac(join_label));
        graph_.getCurrentBlock()->setTrueBlock(join_block);

        graph_.setCurrentBlock(join_block);

        set_value(node, std::string{});
        return;
    }

    if (const auto *if_else_node = dynamic_cast<const IfElseNode *>(&node)) {
        const auto *cond = child_at(*if_else_node, 0);
        const auto *stmt = child_at(*if_else_node, 1);
        const auto *else_stmt = child_at(*if_else_node, 2);
        if (cond == nullptr || stmt == nullptr || else_stmt == nullptr) {
            set_value(node, std::string{});
            return;
        }

        auto *true_block = graph_.newBlock();
        auto *false_block = graph_.newBlock();
        auto *join_block = graph_.newBlock();

        const auto &false_label = false_block->getName();
        const auto &join_label = join_block->getName();

        const auto cond_name = eval(*cond);
        graph_.addInstruction(new CondJumpTac(false_label, cond_name));
        graph_.addInstruction(new JumpTac(true_block->getName()));

        auto *current_block = graph_.getCurrentBlock();
        current_block->setTrueBlock(true_block);
        current_block->setFalseBlock(false_block);

        graph_.setCurrentBlock(true_block);
        (void)eval(*stmt);
        graph_.addInstruction(new JumpTac(join_label));
        graph_.getCurrentBlock()->setTrueBlock(join_block);

        graph_.setCurrentBlock(false_block);
        (void)eval(*else_stmt);
        graph_.addInstruction(new JumpTac(join_label));
        graph_.getCurrentBlock()->setTrueBlock(join_block);

        graph_.setCurrentBlock(join_block);

        set_value(node, std::string{});
        return;
    }

    if (const auto *while_node = dynamic_cast<const WhileNode *>(&node)) {
        const auto *cond = child_at(*while_node, 0);
        const auto *stmt = child_at(*while_node, 1);
        if (cond == nullptr || stmt == nullptr) {
            set_value(node, std::string{});
            return;
        }

        auto *header_block = graph_.newBlock();
        const auto &header_label = header_block->getName();
        auto *body_block = graph_.newBlock();
        body_block->setTrueBlock(header_block);
        auto *join_block = graph_.newBlock();

        auto *current_block = graph_.getCurrentBlock();
        current_block->setTrueBlock(header_block);
        graph_.addInstruction(new JumpTac(header_label));

        graph_.setCurrentBlock(header_block);
        const auto cond_name = eval(*cond);
        auto *condition_block = graph_.getCurrentBlock();
        graph_.addInstruction(
            new CondJumpTac(join_block->getName(), cond_name));
        graph_.addInstruction(new JumpTac(body_block->getName()));

        graph_.setCurrentBlock(body_block);
        (void)eval(*stmt);
        graph_.getCurrentBlock()->setTrueBlock(header_block);
        graph_.addInstruction(new JumpTac(header_label));

        condition_block->setTrueBlock(body_block);
        condition_block->setFalseBlock(join_block);

        graph_.setCurrentBlock(join_block);

        set_value(node, std::string{});
        return;
    }

    if (const auto *method_body_node =
            dynamic_cast<const MethodBodyNode *>(&node)) {
        const auto *body = child_at(*method_body_node, 0);
        const auto *return_value = child_at(*method_body_node, 1);
        if (body == nullptr || return_value == nullptr) {
            set_value(node, std::string{});
            return;
        }

        (void)eval(*body);
        const auto name = eval(*return_value);
        graph_.addInstruction(new ReturnTac(name));
        set_value(node, name);
        return;
    }

    if (const auto *return_only_method_body_node =
            dynamic_cast<const ReturnOnlyMethodBodyNode *>(&node)) {
        const auto *return_value = child_at(*return_only_method_body_node, 0);
        if (return_value == nullptr) {
            set_value(node, std::string{});
            return;
        }

        const auto name = eval(*return_value);
        graph_.addInstruction(new ReturnTac(name));
        set_value(node, name);
        return;
    }

    visit_generic(node);
}

IRGenerationResult generate_ir(const Node &root, CFG &graph, SymbolTable &table,
                               lexing::DiagnosticSink *sink) {
    while (table.getParentScope() != nullptr) {
        table.exitScope();
    }

    IRGenerationVisitor visitor(graph, table, sink);
    root.accept(visitor);
    return visitor.result();
}
