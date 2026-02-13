#include "ir/passes/IRPassManager.hpp"

#include <memory>
#include <utility>

#include "ir/CFG.hpp"
#include "ir/passes/IRPass.hpp"

void IRPassManager::addPass(std::unique_ptr<IRPass> pass) {
    passes_.push_back(std::move(pass));
}

bool IRPassManager::run(CFG &graph) const {
    bool changed = false;
    for (const auto &pass : passes_) {
        changed = pass->run(graph) || changed;
    }
    return changed;
}
