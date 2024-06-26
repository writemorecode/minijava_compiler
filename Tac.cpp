#include "Tac.hpp"
#include "BytecodeMethodBlock.hpp"
#include <iostream>

void Tac::print(std::ostream &os) const {
    os << result << " := " << lhs << " " << op << " " << rhs << "\n";
}

void NotTac::print(std::ostream &os) const {
    os << result << " := ! " << rhs << "\n";
}

void CopyTac::print(std::ostream &os) const {
    os << result << " := " << rhs << "\n";
}

void CopyTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(rhsOp).store(result);
}

void ArrayCopyTac::print(std::ostream &os) const {
    os << result << "[" << lhs << "]"
       << " := " << rhs << "\n";
}

void ArrayAccessTac::print(std::ostream &os) const {
    os << result << " := " << lhs << "[" << rhs << "]\n";
}

void ArrayLengthTac::print(std::ostream &os) const {
    os << result << " := length " << rhs << "\n";
}

void NewTac::print(std::ostream &os) const {
    os << result << " := new " << rhs << "\n";
}

void NewArrayTac::print(std::ostream &os) const {
    os << result << " := new int, " << rhs << "\n";
}

void JumpTac::print(std::ostream &os) const { os << "goto " << result << "\n"; }
void JumpTac::generateBytecode(BytecodeMethodBlock &block) {
    block.jump(result);
}

void CondJumpTac::print(std::ostream &os) const {
    os << "iffalse " << lhs << " goto " << rhs << "\n";
}
void CondJumpTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(lhs).cjump(rhs);
}

void MethodCallTac::print(std::ostream &os) const {
    os << result << " := call " << lhs << ", " << rhs << "\n";
}
void MethodCallTac::generateBytecode(BytecodeMethodBlock &block) {
    block.call(lhs + "." + op).store(result);
}

void ParamTac::print(std::ostream &os) const { os << "param " << rhs << "\n"; }
void ParamTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(rhsOp);
}

void ReturnTac::print(std::ostream &os) const {
    os << "return " << rhs << "\n";
}
void ReturnTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(rhsOp).ret();
}

void PrintTac::print(std::ostream &os) const { os << "print " << rhs << "\n"; }
void PrintTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(rhsOp).write();
}

void NotTac::generateBytecode(BytecodeMethodBlock &block) {
    block.push(rhsOp).l_not().store(result);
}
