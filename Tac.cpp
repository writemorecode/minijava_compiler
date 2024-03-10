#include "Tac.hpp"
#include <iostream>

void Tac::print() const {
    std::cout << result << " := " << lhs << " " << op << " " << rhs << "\n";
}

void ExpressionTac::print() const {
    std::cout << result << " := " << lhs << " " << op << " " << rhs << "\n";
}

void CopyTac::print() const { std::cout << result << " := " << lhs << "\n"; }
