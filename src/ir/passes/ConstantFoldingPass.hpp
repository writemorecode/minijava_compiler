#ifndef CONSTANT_FOLDING_PASS_HPP
#define CONSTANT_FOLDING_PASS_HPP

#include <string_view>

#include "ir/passes/IRPass.hpp"

class ConstantFoldingPass final : public IRPass {
  public:
    [[nodiscard]] std::string_view name() const override {
        return "constant-folding";
    }
    bool run(CFG &graph) override;
};

#endif
