#include "Tac.hpp"
#include <iostream>

void Tac::print(std::ostream &os) const {
    os << result << " := " << lhs << " " << op << " " << rhs << "\n";
}

void ExpressionTac::print(std::ostream &os) const {
    os << result << " := " << lhs << " " << op << " " << rhs << "\n";
}

void UnaryExpressionTac::print(std::ostream &os) const {
    os << result << " := " << op << " " << rhs << "\n";
}

void CopyTac::print(std::ostream &os) const {
    os << result << " := " << lhs << "\n";
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

void JumpTac::print(std::ostream &os) const {
    os << op << " " << result << "\n";
}

void CondJumpTac::print(std::ostream &os) const {
    os << "iffalse " << lhs << " goto " << rhs << "\n";
}

void ParamTac::print(std::ostream &os) const { os << "param " << rhs << "\n"; }

void MethodCallTac::print(std::ostream &os) const {
    os << result << " := call " << lhs << ", " << rhs << "\n";
}

void ReturnTac::print(std::ostream &os) const {
    os << "return " << result << "\n";
}

void PrintTac::print(std::ostream &os) const {
    os << "print " << result << "\n";
}
