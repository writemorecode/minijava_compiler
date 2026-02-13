#ifndef CONDITIONAL_JUMP_FOLDING_PASS_HPP
#define CONDITIONAL_JUMP_FOLDING_PASS_HPP

#include <string_view>

#include "ir/passes/IRPass.hpp"

class ConditionalJumpFoldingPass final : public IRPass {
  public:
    [[nodiscard]] std::string_view name() const override {
        return "conditional-jump-folding";
    }
    bool run(CFG &graph) override;
};

#endif
