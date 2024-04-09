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

void StackParameterInstruction::serialize(Serializer &serializer) const {
    serializer.writeOpcode(opcode);
};
void IntegerParameterInstruction::serialize(Serializer &serializer) const {
    serializer.writeOpcode(opcode);
    serializer.writeInteger(param);
};
void StringParameterInstruction::serialize(Serializer &serializer) const {
    serializer.writeOpcode(opcode);
    serializer.writeString(param);
};
