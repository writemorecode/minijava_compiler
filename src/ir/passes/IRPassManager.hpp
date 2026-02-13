#ifndef IR_PASS_MANAGER_HPP
#define IR_PASS_MANAGER_HPP

#include <memory>
#include <vector>

class CFG;
class IRPass;

class IRPassManager {
  public:
    void addPass(std::unique_ptr<IRPass> pass);
    [[nodiscard]] bool run(CFG &graph) const;

  private:
    std::vector<std::unique_ptr<IRPass>> passes_;
};

#endif
