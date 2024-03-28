#include "BytecodeInstruction.hpp"

void StackParameterInstruction::print(std::ostream &os) const {
    os << getCounter() << '\t';
    os << mnemonics[getOpcode()] << '\n';
};
void IntegerParameterInstruction::print(std::ostream &os) const {
    os << getCounter() << '\t';
    os << mnemonics[getOpcode()] << '\t' << param << '\n';
};
void StringParameterInstruction::print(std::ostream &os) const {
    os << getCounter() << '\t';
    os << mnemonics[getOpcode()] << '\t' << param << '\n';
};
