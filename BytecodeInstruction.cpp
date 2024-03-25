#include "BytecodeInstruction.hpp"

void StackParameterInstruction::print(std::ostream &os) const {
    os << mnemonics[opcode] << '\n';
};
void IntegerParameterInstruction::print(std::ostream &os) const {
    os << mnemonics[opcode] << '\t' << param << '\n';
};
void StringParameterInstruction::print(std::ostream &os) const {
    os << mnemonics[opcode] << '\t' << param << '\n';
};
