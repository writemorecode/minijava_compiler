#include "BooleanTac.hpp"

void AndTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).l_and().store(result);
}

void OrTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).l_or().store(result);
}
