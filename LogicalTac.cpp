#include "LogicalTac.hpp"

void LessThanTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::LT));
}
void GreaterThanTac::generateBytecode(BytecodeMethodBlock &block) {
    // Note that operands are pushed in reverse order:
    // A > a iff a < A
    block.addOperandPushInstruction(rhsOp);
    block.addOperandPushInstruction(lhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::LT));
}
void EqualToTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    // A == B iff A - B = 0
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::SUB));
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::NOT));
}
