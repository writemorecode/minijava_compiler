#include "BooleanTac.hpp"

void AndTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::AND));
}

void OrTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(lhsOp);
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::OR));
}
