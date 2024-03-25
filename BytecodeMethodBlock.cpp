#include "BytecodeMethodBlock.hpp"

using Operand = std::variant<std::string, int>;

void BytecodeMethodBlock::addBytecodeInstruction(BytecodeInstruction *instr) {
    instructions.emplace_back(instr);
}

void BytecodeMethodBlock::addOperandPushInstruction(const Operand &operand) {
    if (const auto *ptr = std::get_if<int>(&operand)) {
        addBytecodeInstruction(
            new IntegerParameterInstruction(Opcode::CONST, *ptr));
    } else if (const auto *ptr = std::get_if<std::string>(&operand)) {
        addBytecodeInstruction(
            new StringParameterInstruction(Opcode::LOAD, *ptr));
    }
}

void BytecodeMethodBlock::print(std::ostream &os) const {
    for (const auto &instruction : instructions) {
        instruction->print(os);
    }
}
