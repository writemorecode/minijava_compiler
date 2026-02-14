#include "ir/passes/UnreachableBlockEliminationPass.hpp"

#include "ir/CFG.hpp"

bool UnreachableBlockEliminationPass::run(CFG &graph) {
    return graph.removeUnreachableBlocks();
}
