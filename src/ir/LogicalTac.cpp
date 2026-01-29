#include "ir/LogicalTac.hpp"

void LessThanTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).less_than().store(result);
}
void GreaterThanTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).greater_than().store(result);
}
void EqualToTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).equal_to().store(result);
}
