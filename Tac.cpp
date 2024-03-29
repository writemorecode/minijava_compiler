#include "Tac.hpp"
#include "BytecodeMethodBlock.hpp"
#include <iostream>

void Tac::print(std::ostream &os) const {
    os << result << " := " << lhs << " " << op << " " << rhs << "\n";
}

void UnaryExpressionTac::print(std::ostream &os) const {
    os << result << " := " << op << " " << rhs << "\n";
}

void CopyTac::print(std::ostream &os) const {
    os << result << " := " << rhs << "\n";
}

void CopyTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addBytecodeInstruction(
        new StringParameterInstruction(Opcode::STORE, result));
}

void ArrayCopyTac::print(std::ostream &os) const {
    os << result << "[" << lhs << "]"
       << " := " << rhs << "\n";
}

void ArrayAccessTac::print(std::ostream &os) const {
    os << result << " := " << lhs << "[" << rhs << "]\n";
}

void NewTac::print(std::ostream &os) const {
    os << result << " := new " << rhs << "\n";
}

void NewArrayTac::print(std::ostream &os) const {
    os << result << " := new int, " << rhs << "\n";
}

void CondJumpTac::print(std::ostream &os) const {
    os << "iffalse " << lhs << " goto " << rhs << "\n";
}

void MethodCallTac::print(std::ostream &os) const {
    os << result << " := call " << lhs << ", " << rhs << "\n";
}

void JumpTac::print(std::ostream &os) const { os << "goto " << result << "\n"; }

void ParamTac::print(std::ostream &os) const { os << "param " << rhs << "\n"; }

void ReturnTac::print(std::ostream &os) const {
    os << "return " << rhs << "\n";
}
void ReturnTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addBytecodeInstruction(
        new StringParameterInstruction(Opcode::LOAD, rhs));
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::RET));
}

void PrintTac::print(std::ostream &os) const { os << "print " << rhs << "\n"; }
void PrintTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::PRINT));
}

void NotTac::generateBytecode(BytecodeMethodBlock &block) {
    block.addOperandPushInstruction(rhsOp);
    block.addBytecodeInstruction(new StackParameterInstruction(Opcode::NOT));
}
