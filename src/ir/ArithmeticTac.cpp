#include "ir/ArithmeticTac.hpp"

void AddTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).add().store(result);
}
void SubtractTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).subtract().store(result);
}
void MultiplyTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).multiply().store(result);
}
void DivideTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhsOp).push(rhsOp).divide().store(result);
}
