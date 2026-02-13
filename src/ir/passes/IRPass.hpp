#ifndef IR_PASS_HPP
#define IR_PASS_HPP

#include <string_view>

class CFG;

class IRPass {
  public:
    virtual ~IRPass() = default;

    [[nodiscard]] virtual std::string_view name() const = 0;
    virtual bool run(CFG &graph) = 0;
};

#endif
