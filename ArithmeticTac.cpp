#include "ArithmeticTac.hpp"
#include "BytecodeInstruction.hpp"

void AddTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::ADD));
    block.addStoreInstruction(result);
}
void SubtractTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::SUB));
    block.addStoreInstruction(result);
}
void MultiplyTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::MUL));
    block.addStoreInstruction(result);
}
void DivideTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::DIV));
    block.addStoreInstruction(result);
}
