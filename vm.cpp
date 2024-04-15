#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "Opcode.hpp"
#include "serialize.hpp"

struct Instruction {
    Opcode op;
    size_t argNumber;
    std::string argString;
};

class VM {
    std::unordered_map<std::string, size_t> variables;
    std::stack<size_t> dataStack;
    std::vector<Instruction> instructions;

    size_t pc = 0;

  public:
    Instruction getNextInstruction() { return instructions[pc++]; }
    void addInstruction(const auto &instr) { instructions.push_back(instr); }
    void addVariable(const std::string &name) { variables[name] = 0; }
    void setVariable(const std::string &name, size_t value) {
        variables[name] = value;
    }
    size_t getVariable(const std::string &name) const {
        if (const auto &it = variables.find(name); it != variables.end()) {
            return it->second;
        }
        // FIXME: Find a way to remove this exception
        throw std::invalid_argument("variable not found");
    }
    void push(size_t value) { dataStack.push(value); }
    void push(const std::string &name) { push(getVariable(name)); }
    size_t pop() {
        if (dataStack.empty()) {
            throw std::runtime_error("empty data stack");
        }
        auto value = dataStack.top();
        dataStack.pop();
        return value;
    }
    bool isEmpty() { return dataStack.empty(); }

    void run();
};

void VM::run() {
    while (true) {
        const auto instruction = getNextInstruction();
        const auto op = instruction.op;
        switch (op) {
        case Opcode::STOP: {
            std::cout << "Reached end of program.\n";
            return;
        }
        case Opcode::RET: {
            std::cout << "Returning from method.\n";
            break;
        }
        case Opcode::PRINT: {
            const auto value = pop();
            std::cout << "Print: " << value << "\n";
            break;
        }
        case Opcode::ADD: {
            auto x = pop();
            auto y = pop();
            push(x + y);
            break;
        }
        case Opcode::SUB: {
            auto x = pop();
            auto y = pop();
            push(y - x);
            break;
        }
        case Opcode::MUL: {
            auto x = pop();
            auto y = pop();
            push(x * y);
            break;
        }
        case Opcode::DIV: {
            auto x = pop();
            auto y = pop();
            push(y / x);
            break;
        }
        case Opcode::LT: {
            auto x = pop();
            auto y = pop();
            push(x > y ? 1 : 0);
            break;
        }
        case Opcode::GT: {
            auto x = pop();
            auto y = pop();
            push(x < y ? 1 : 0);
            break;
        }
        case Opcode::AND: {
            auto x = pop();
            auto y = pop();
            push(x * y == 0 ? 0 : 1);
            break;
        }
        case Opcode::OR: {
            auto x = pop();
            auto y = pop();
            push(x + y == 0 ? 0 : 1);
            break;
        }
        case Opcode::EQ: {
            auto x = pop();
            auto y = pop();
            push(x == y ? 1 : 0);
            break;
        }
        case Opcode::NOT: {
            auto x = pop();
            push(x == 0 ? 1 : 0);
            break;
        }
        case Opcode::CONST: {
            push(instruction.argNumber);
            break;
        }
        case Opcode::LOAD: {
            push(instruction.argString);
            break;
        }
        case Opcode::STORE: {
            setVariable(instruction.argString, pop());
            break;
        }
        default: {
            std::cerr << "Invalid opcode " << op << "\n";
            return;
        }
        };
    }
}

[[nodiscard]] VM readProgram(Deserializer &reader) {
    size_t methodCount = reader.readInteger();
    std::cout << "Program has " << methodCount << " methods.\n";

    VM vm;

    auto methodName = reader.readString();
    std::cout << "Method: " << std::quoted(methodName) << "\n";

    const auto variables = reader.readStringVector();
    for (const auto &variable : variables) {
        vm.addVariable(variable);
    }
    auto instructionCount = reader.readInteger();
    std::cout << "Instruction count: " << instructionCount << "\n";

    for (size_t i = 0; i < instructionCount; i++) {
        Instruction current;
        current.op = reader.readOpcode();

        switch (current.op) {
        case Opcode::JMP:
        case Opcode::CJMP:
        case Opcode::CALL:
        case Opcode::LOAD:
        case Opcode::STORE: {
            current.argString = reader.readString();
            break;
        }
        case Opcode::CONST: {
            current.argNumber = reader.readInteger();
            break;
        }
        default: {
            break;
        }
        }
        vm.addInstruction(current);
    }

    return vm;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [bytecode program]\n";
        return EXIT_FAILURE;
    }

    std::string_view filename{argv[1]};
    std::ifstream programFile(filename, std::ios::binary);
    if (programFile.eof() || programFile.eof()) {
        std::cerr << "Error: Unable to open file " << filename << ".\n";
        return EXIT_FAILURE;
    }

    Deserializer reader(programFile);

    auto vm = readProgram(reader);
    try {
        vm.run();
    } catch (std::exception &exc) {
        std::cerr << "VM threw exception: " << exc.what() << "\n";
    }
}
