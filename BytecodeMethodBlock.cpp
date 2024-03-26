#include "BytecodeMethodBlock.hpp"
#include "BytecodeInstruction.hpp"
#include <memory>

using Operand = std::variant<std::string, int>;

void BytecodeMethodBlock::print(std::ostream &os) const {
    for (const auto &instruction : instructions) {
        instruction->print(os);
    }
}

void BytecodeMethodBlock::addBytecodeInstruction(BytecodeInstruction *instr) {
    instructions.emplace_back(instr);
}

BytecodeMethodBlock &BytecodeMethodBlock::push(const Operand &operand) {
    if (const auto *ptr = std::get_if<int>(&operand)) {
        addBytecodeInstruction(
            new IntegerParameterInstruction(Opcode::CONST, *ptr));
    } else if (const auto *ptr = std::get_if<std::string>(&operand)) {
        addBytecodeInstruction(
            new StringParameterInstruction(Opcode::LOAD, *ptr));
    }
    return *this;
}

BytecodeMethodBlock &BytecodeMethodBlock::store(const std::string &result) {
    addBytecodeInstruction(
        new StringParameterInstruction(Opcode::STORE, result));
    return *this;
}

BytecodeMethodBlock &BytecodeMethodBlock::add() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::ADD));
    return *this;
}
BytecodeMethodBlock &BytecodeMethodBlock::subtract() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::SUB));
    return *this;
}
BytecodeMethodBlock &BytecodeMethodBlock::multiply() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::MUL));
    return *this;
}
BytecodeMethodBlock &BytecodeMethodBlock::divide() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::DIV));
    return *this;
}

BytecodeMethodBlock &BytecodeMethodBlock::less_than() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::LT));
    return *this;
}

BytecodeMethodBlock &BytecodeMethodBlock::l_and() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::AND));
    return *this;
}
BytecodeMethodBlock &BytecodeMethodBlock::l_or() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::OR));
    return *this;
}
BytecodeMethodBlock &BytecodeMethodBlock::l_not() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::NOT));
    return *this;
}
BytecodeMethodBlock &BytecodeMethodBlock::ret() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::RET));
    return *this;
}
BytecodeMethodBlock &BytecodeMethodBlock::write() {
    addBytecodeInstruction(new StackParameterInstruction(Opcode::PRINT));
    return *this;
}
