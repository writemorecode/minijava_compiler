#include "ir/passes/ConstantFoldingPass.hpp"

#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ir/ArithmeticTac.hpp"
#include "ir/BBlock.hpp"
#include "ir/BooleanTac.hpp"
#include "ir/CFG.hpp"
#include "ir/LogicalTac.hpp"
#include "ir/Tac.hpp"

namespace {
using ConstantEnvironment = std::unordered_map<std::string, std::int64_t>;

[[nodiscard]] std::optional<int> to_ir_immediate(std::int64_t value) {
    if (value < std::numeric_limits<int>::min() ||
        value > std::numeric_limits<int>::max()) {
        return std::nullopt;
    }
    return static_cast<int>(value);
}

[[nodiscard]] std::optional<std::int64_t>
resolve_constant_operand(const Operand &operand,
                         const ConstantEnvironment &environment) {
    if (const auto *immediate = std::get_if<int>(&operand)) {
        return *immediate;
    }
    if (const auto *name = std::get_if<std::string>(&operand)) {
        if (const auto it = environment.find(*name); it != environment.end()) {
            return it->second;
        }
    }
    return std::nullopt;
}

bool substitute_lhs_if_constant(Tac &instruction,
                                const ConstantEnvironment &environment) {
    const auto *name = std::get_if<std::string>(&instruction.getLhsOperand());
    if (name == nullptr) {
        return false;
    }
    const auto it = environment.find(*name);
    if (it == environment.end()) {
        return false;
    }
    const auto value = to_ir_immediate(it->second);
    if (!value.has_value()) {
        return false;
    }
    instruction.setLhsOperand(*value);
    return true;
}

bool substitute_rhs_if_constant(Tac &instruction,
                                const ConstantEnvironment &environment) {
    const auto *name = std::get_if<std::string>(&instruction.getRhsOperand());
    if (name == nullptr) {
        return false;
    }
    const auto it = environment.find(*name);
    if (it == environment.end()) {
        return false;
    }
    const auto value = to_ir_immediate(it->second);
    if (!value.has_value()) {
        return false;
    }
    instruction.setRhsOperand(*value);
    return true;
}

template <typename Operator>
[[nodiscard]] std::optional<std::int64_t>
fold_binary(const Tac &instruction, const ConstantEnvironment &environment,
            Operator op) {
    const auto lhs =
        resolve_constant_operand(instruction.getLhsOperand(), environment);
    const auto rhs =
        resolve_constant_operand(instruction.getRhsOperand(), environment);
    if (!lhs.has_value() || !rhs.has_value()) {
        return std::nullopt;
    }
    return op(*lhs, *rhs);
}

[[nodiscard]] std::optional<std::int64_t>
try_fold_instruction(const Tac &instruction,
                     const ConstantEnvironment &environment) {
    if (dynamic_cast<const AddTac *>(&instruction) != nullptr) {
        return fold_binary(
            instruction, environment,
            [](std::int64_t lhs, std::int64_t rhs) { return lhs + rhs; });
    }
    if (dynamic_cast<const SubtractTac *>(&instruction) != nullptr) {
        return fold_binary(
            instruction, environment,
            [](std::int64_t lhs, std::int64_t rhs) { return lhs - rhs; });
    }
    if (dynamic_cast<const MultiplyTac *>(&instruction) != nullptr) {
        return fold_binary(
            instruction, environment,
            [](std::int64_t lhs, std::int64_t rhs) { return lhs * rhs; });
    }
    if (dynamic_cast<const DivideTac *>(&instruction) != nullptr) {
        const auto lhs =
            resolve_constant_operand(instruction.getLhsOperand(), environment);
        const auto rhs =
            resolve_constant_operand(instruction.getRhsOperand(), environment);
        if (!lhs.has_value() || !rhs.has_value() || *rhs == 0) {
            return std::nullopt;
        }
        return *lhs / *rhs;
    }
    if (dynamic_cast<const LessThanTac *>(&instruction) != nullptr) {
        return fold_binary(instruction, environment,
                           [](std::int64_t lhs, std::int64_t rhs) {
                               return lhs < rhs ? 1 : 0;
                           });
    }
    if (dynamic_cast<const GreaterThanTac *>(&instruction) != nullptr) {
        return fold_binary(instruction, environment,
                           [](std::int64_t lhs, std::int64_t rhs) {
                               return lhs > rhs ? 1 : 0;
                           });
    }
    if (dynamic_cast<const EqualToTac *>(&instruction) != nullptr) {
        return fold_binary(instruction, environment,
                           [](std::int64_t lhs, std::int64_t rhs) {
                               return lhs == rhs ? 1 : 0;
                           });
    }
    if (dynamic_cast<const AndTac *>(&instruction) != nullptr) {
        return fold_binary(instruction, environment,
                           [](std::int64_t lhs, std::int64_t rhs) {
                               return (lhs != 0 && rhs != 0) ? 1 : 0;
                           });
    }
    if (dynamic_cast<const OrTac *>(&instruction) != nullptr) {
        return fold_binary(instruction, environment,
                           [](std::int64_t lhs, std::int64_t rhs) {
                               return (lhs != 0 || rhs != 0) ? 1 : 0;
                           });
    }
    if (dynamic_cast<const NotTac *>(&instruction) != nullptr) {
        const auto rhs =
            resolve_constant_operand(instruction.getRhsOperand(), environment);
        if (!rhs.has_value()) {
            return std::nullopt;
        }
        return *rhs == 0 ? 1 : 0;
    }
    return std::nullopt;
}

bool substitute_constants_in_instruction(
    Tac &instruction, const ConstantEnvironment &environment) {
    bool changed = false;

    if (dynamic_cast<AddTac *>(&instruction) != nullptr ||
        dynamic_cast<SubtractTac *>(&instruction) != nullptr ||
        dynamic_cast<MultiplyTac *>(&instruction) != nullptr ||
        dynamic_cast<DivideTac *>(&instruction) != nullptr ||
        dynamic_cast<LessThanTac *>(&instruction) != nullptr ||
        dynamic_cast<GreaterThanTac *>(&instruction) != nullptr ||
        dynamic_cast<EqualToTac *>(&instruction) != nullptr ||
        dynamic_cast<AndTac *>(&instruction) != nullptr ||
        dynamic_cast<OrTac *>(&instruction) != nullptr ||
        dynamic_cast<ArrayAccessTac *>(&instruction) != nullptr ||
        dynamic_cast<ArrayCopyTac *>(&instruction) != nullptr) {
        changed =
            substitute_lhs_if_constant(instruction, environment) || changed;
        changed =
            substitute_rhs_if_constant(instruction, environment) || changed;
        return changed;
    }

    if (dynamic_cast<NotTac *>(&instruction) != nullptr ||
        dynamic_cast<CopyTac *>(&instruction) != nullptr ||
        dynamic_cast<ArrayLengthTac *>(&instruction) != nullptr ||
        dynamic_cast<NewArrayTac *>(&instruction) != nullptr ||
        dynamic_cast<ParamTac *>(&instruction) != nullptr ||
        dynamic_cast<ReturnTac *>(&instruction) != nullptr ||
        dynamic_cast<PrintTac *>(&instruction) != nullptr) {
        changed =
            substitute_rhs_if_constant(instruction, environment) || changed;
        return changed;
    }

    if (dynamic_cast<CondJumpTac *>(&instruction) != nullptr ||
        dynamic_cast<MethodCallTac *>(&instruction) != nullptr) {
        changed =
            substitute_lhs_if_constant(instruction, environment) || changed;
        return changed;
    }

    return changed;
}

[[nodiscard]] std::optional<std::string>
defined_variable(const Tac &instruction) {
    if (dynamic_cast<const AddTac *>(&instruction) != nullptr ||
        dynamic_cast<const SubtractTac *>(&instruction) != nullptr ||
        dynamic_cast<const MultiplyTac *>(&instruction) != nullptr ||
        dynamic_cast<const DivideTac *>(&instruction) != nullptr ||
        dynamic_cast<const LessThanTac *>(&instruction) != nullptr ||
        dynamic_cast<const GreaterThanTac *>(&instruction) != nullptr ||
        dynamic_cast<const EqualToTac *>(&instruction) != nullptr ||
        dynamic_cast<const AndTac *>(&instruction) != nullptr ||
        dynamic_cast<const OrTac *>(&instruction) != nullptr ||
        dynamic_cast<const NotTac *>(&instruction) != nullptr ||
        dynamic_cast<const CopyTac *>(&instruction) != nullptr ||
        dynamic_cast<const ArrayAccessTac *>(&instruction) != nullptr ||
        dynamic_cast<const ArrayLengthTac *>(&instruction) != nullptr ||
        dynamic_cast<const NewTac *>(&instruction) != nullptr ||
        dynamic_cast<const NewArrayTac *>(&instruction) != nullptr ||
        dynamic_cast<const MethodCallTac *>(&instruction) != nullptr) {
        return instruction.getResult();
    }
    return std::nullopt;
}

bool process_block(BBlock &block) {
    ConstantEnvironment environment;
    bool changed = false;

    auto &instructions = block.getInstructions();
    for (auto &instruction_ptr : instructions) {
        auto &instruction = *instruction_ptr;
        changed =
            substitute_constants_in_instruction(instruction, environment) ||
            changed;

        const auto folded_value =
            try_fold_instruction(instruction, environment);
        if (folded_value.has_value()) {
            const auto folded_immediate = to_ir_immediate(*folded_value);
            if (folded_immediate.has_value()) {
                const auto result = instruction.getResult();
                instruction_ptr =
                    std::make_unique<CopyTac>(*folded_immediate, result);
                environment[result] = *folded_value;
                changed = true;
                continue;
            }
        }

        if (dynamic_cast<const MethodCallTac *>(&instruction) != nullptr) {
            environment.clear();
            if (const auto result = defined_variable(instruction);
                result.has_value()) {
                environment.erase(*result);
            }
            continue;
        }

        if (const auto *copy = dynamic_cast<const CopyTac *>(&instruction);
            copy != nullptr) {
            const auto constant =
                resolve_constant_operand(copy->getRhsOperand(), environment);
            if (constant.has_value()) {
                environment[instruction.getResult()] = *constant;
            } else {
                environment.erase(instruction.getResult());
            }
            continue;
        }

        if (const auto result = defined_variable(instruction);
            result.has_value()) {
            environment.erase(*result);
        }
    }

    return changed;
}

bool process_method_root(BBlock *root) {
    std::vector<BBlock *> stack{root};
    std::unordered_set<BBlock *> visited;
    bool changed = false;

    while (!stack.empty()) {
        auto *block = stack.back();
        stack.pop_back();
        if (!visited.insert(block).second) {
            continue;
        }

        changed = process_block(*block) || changed;
        if (block->hasTrueBlock()) {
            stack.push_back(block->getTrueBlock());
        }
        if (block->hasFalseBlock()) {
            stack.push_back(block->getFalseBlock());
        }
    }

    return changed;
}

} // namespace

bool ConstantFoldingPass::run(CFG &graph) {
    bool changed = false;
    for (auto *root : graph.getMethodRoots()) {
        if (root == nullptr) {
            continue;
        }
        changed = process_method_root(root) || changed;
    }
    return changed;
}
