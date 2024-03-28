#include "BytecodeInstruction.hpp"

void StackParameterInstruction::print(std::ostream &os) const {
    os << mnemonic << '\n';
};
void IntegerParameterInstruction::print(std::ostream &os) const {
    os << mnemonic << '\t' << param << '\n';
};
void StringParameterInstruction::print(std::ostream &os) const {
    os << mnemonic << '\t' << param << '\n';
};
