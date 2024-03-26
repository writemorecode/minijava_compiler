#include "LogicalTac.hpp"

void LessThanTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).less_than().store(result);
}
void GreaterThanTac::generateBytecode(BytecodeMethodBlock &block) {
    // Note that operands are pushed in reverse order:
    // A > a iff a < A
    block.push(rhsOp).push(lhsOp).less_than().store(result);
}
void EqualToTac::generateBytecode(BytecodeMethodBlock &block) {
    // A == B iff A - B = 0
    block.push(lhsOp).push(rhsOp).subtract().l_and().store(result);
}
