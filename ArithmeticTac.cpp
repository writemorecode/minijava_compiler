#include "ArithmeticTac.hpp"
#include "BytecodeInstruction.hpp"

void AddTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::ADD));
    block.addBytecodeInstruction(
        new StringParameterInstruction(Opcode::STORE, result));
}
void SubtractTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::SUB));
    block.addBytecodeInstruction(
        new StringParameterInstruction(Opcode::STORE, result));
}
void MultiplyTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::MUL));
    block.addBytecodeInstruction(
        new StringParameterInstruction(Opcode::STORE, result));
}
void DivideTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::DIV));
    block.addBytecodeInstruction(
        new StringParameterInstruction(Opcode::STORE, result));
}
