#include "ir/passes/ConditionalJumpFoldingPass.hpp"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "ir/BBlock.hpp"
#include "ir/CFG.hpp"
#include "ir/Tac.hpp"

namespace {

using BlockNameMap = std::unordered_map<std::string, BBlock *>;

[[nodiscard]] std::optional<std::string> as_label(const Operand &operand) {
    const auto *label = std::get_if<std::string>(&operand);
    if (label == nullptr) {
        return std::nullopt;
    }
    return *label;
}

void collect_method_blocks(BBlock *root, std::vector<BBlock *> &blocks,
                           BlockNameMap &block_names) {
    std::vector<BBlock *> stack{root};
    std::unordered_set<BBlock *> visited;

    while (!stack.empty()) {
        auto *block = stack.back();
        stack.pop_back();

        if (!visited.insert(block).second) {
            continue;
        }

        blocks.push_back(block);
        block_names[block->getName()] = block;

        if (block->hasTrueBlock()) {
            stack.push_back(block->getTrueBlock());
        }
        if (block->hasFalseBlock()) {
            stack.push_back(block->getFalseBlock());
        }
    }
}

struct FoldDecision {
    std::string target_label;
    BBlock *target_block = nullptr;
};

[[nodiscard]] const JumpTac *
next_jump_instruction(const std::vector<std::unique_ptr<Tac>> &instructions,
                      std::size_t index) {
    if (index + 1 >= instructions.size()) {
        return nullptr;
    }
    return dynamic_cast<JumpTac *>(instructions[index + 1].get());
}

[[nodiscard]] std::optional<std::string>
resolve_target_label(const CondJumpTac &conditional_jump, int condition_value,
                     BBlock &block,
                     const std::vector<std::unique_ptr<Tac>> &instructions,
                     std::size_t index) {
    if (condition_value == 0) {
        return as_label(conditional_jump.getRhsOperand());
    }

    if (const auto *next_jump = next_jump_instruction(instructions, index);
        next_jump != nullptr) {
        return next_jump->getResult();
    }
    if (block.hasTrueBlock()) {
        return block.getTrueBlock()->getName();
    }
    return std::nullopt;
}

[[nodiscard]] BBlock *resolve_target_block(BBlock &block,
                                           int condition_value,
                                           const std::string &target_label,
                                           const BlockNameMap &block_names) {
    if (condition_value == 0 && block.hasFalseBlock()) {
        return block.getFalseBlock();
    }
    if (condition_value != 0 && block.hasTrueBlock()) {
        return block.getTrueBlock();
    }

    if (const auto it = block_names.find(target_label); it != block_names.end()) {
        return it->second;
    }
    return nullptr;
}

[[nodiscard]] std::optional<FoldDecision>
build_fold_decision(const CondJumpTac &conditional_jump, int condition_value,
                    BBlock &block,
                    const std::vector<std::unique_ptr<Tac>> &instructions,
                    std::size_t index, const BlockNameMap &block_names) {
    const auto target_label = resolve_target_label(
        conditional_jump, condition_value, block, instructions, index);
    if (!target_label.has_value()) {
        return std::nullopt;
    }

    BBlock *target_block = resolve_target_block(
        block, condition_value, *target_label, block_names);
    if (target_block == nullptr) {
        return std::nullopt;
    }

    return FoldDecision{.target_label = *target_label,
                        .target_block = target_block};
}

bool apply_fold(BBlock &block, std::vector<std::unique_ptr<Tac>> &instructions,
                std::size_t index, const FoldDecision &decision) {
    instructions[index] = std::make_unique<JumpTac>(decision.target_label);

    if (next_jump_instruction(instructions, index) != nullptr) {
        instructions.erase(instructions.begin() + static_cast<long>(index + 1));
    }

    block.setTrueBlock(decision.target_block);
    block.setFalseBlock(nullptr);
    return true;
}

bool process_block(BBlock &block, const BlockNameMap &block_names) {
    auto &instructions = block.getInstructions();
    bool changed = false;

    for (std::size_t index = 0; index < instructions.size(); ++index) {
        const auto *conditional_jump =
            dynamic_cast<CondJumpTac *>(instructions[index].get());
        if (conditional_jump == nullptr) {
            continue;
        }

        const auto *condition_value =
            std::get_if<int>(&conditional_jump->getLhsOperand());
        if (condition_value == nullptr) {
            continue;
        }

        const auto decision = build_fold_decision(
            *conditional_jump, *condition_value, block, instructions, index,
            block_names);
        if (!decision.has_value()) {
            continue;
        }

        changed = apply_fold(block, instructions, index, *decision) || changed;
    }

    return changed;
}

bool process_method_root(BBlock *root) {
    std::vector<BBlock *> blocks;
    BlockNameMap block_names;
    collect_method_blocks(root, blocks, block_names);

    bool changed = false;
    for (auto *block : blocks) {
        changed = process_block(*block, block_names) || changed;
    }
    return changed;
}

} // namespace

bool ConditionalJumpFoldingPass::run(CFG &graph) {
    bool changed = false;
    for (auto *root : graph.getMethodRoots()) {
        if (root == nullptr) {
            continue;
        }
        changed = process_method_root(root) || changed;
    }
    return changed;
}
