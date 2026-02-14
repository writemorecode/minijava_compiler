#ifndef UNREACHABLE_BLOCK_ELIMINATION_PASS_HPP
#define UNREACHABLE_BLOCK_ELIMINATION_PASS_HPP

#include <string_view>

#include "ir/passes/IRPass.hpp"

class UnreachableBlockEliminationPass final : public IRPass {
  public:
    [[nodiscard]] std::string_view name() const override {
        return "unreachable-block-elimination";
    }
    bool run(CFG &graph) override;
};

#endif
