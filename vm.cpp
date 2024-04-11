#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include "Opcode.hpp"
#include "serialize.hpp"

class VM {
    std::unordered_map<std::string, int> variables;
    std::stack<int> dataStack;

  public:
    void addVariable(const std::string &name) { variables[name] = 0; }
    void setVariable(const std::string &name, int value) {
        variables[name] = value;
    }
    int getVariable(const std::string &name) const {
        if (const auto &it = variables.find(name); it != variables.end()) {
            return it->second;
        }
        // FIXME: Find a way to remove this exception
        throw std::invalid_argument("variable not found");
    }
    void push(int value) { dataStack.push(value); }
    void push(const std::string &name) { push(getVariable(name)); }
    int pop() {
        if (dataStack.empty()) {
            throw std::runtime_error("empty data stack");
        }
        int value = dataStack.top();
        dataStack.pop();
        return value;
    }
    bool isEmpty() { return dataStack.empty(); }
};

void runProgram(Deserializer &reader, VM &vm) {
    while (true) {
        const auto op = reader.readOpcode();
        // std::cout << mnemonics[op] << " ";
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
            const auto value = vm.pop();
            std::cout << "Print: " << value << "\n";
            break;
        }
        case Opcode::ADD: {
            auto x = vm.pop();
            auto y = vm.pop();
            vm.push(x + y);
            break;
        }
        case Opcode::SUB: {
            auto x = vm.pop();
            auto y = vm.pop();
            vm.push(y - x);
            break;
        }
        case Opcode::MUL: {
            auto x = vm.pop();
            auto y = vm.pop();
            vm.push(x * y);
            break;
        }
        case Opcode::DIV: {
            auto x = vm.pop();
            auto y = vm.pop();
            vm.push(y / x);
            break;
        }
        case Opcode::LT: {
            auto x = vm.pop();
            auto y = vm.pop();
            vm.push(x > y ? 1 : 0);
            break;
        }
        case Opcode::GT: {
            auto x = vm.pop();
            auto y = vm.pop();
            vm.push(x < y ? 1 : 0);
            break;
        }
        case Opcode::AND: {
            auto x = vm.pop();
            auto y = vm.pop();
            vm.push(x * y == 0 ? 0 : 1);
            break;
        }
        case Opcode::OR: {
            auto x = vm.pop();
            auto y = vm.pop();
            vm.push(x + y == 0 ? 0 : 1);
            break;
        }
        case Opcode::EQ: {
            auto x = vm.pop();
            auto y = vm.pop();
            vm.push(x == y ? 1 : 0);
            break;
        }
        case Opcode::NOT: {
            auto x = vm.pop();
            vm.push(x == 0 ? 1 : 0);
            break;
        }
        case Opcode::JMP:
        case Opcode::CJMP:
        case Opcode::CALL: {
            auto strParam = reader.readString();
            std::cout << "Opcode not yet implemented: " << mnemonics[op] << " "
                      << strParam << "\n";
            break;
        }
        case Opcode::CONST: {
            auto constParam = reader.readInteger();
            vm.push(constParam);
            break;
        }
        case Opcode::LOAD: {
            auto strParam = reader.readString();
            vm.push(strParam);
            break;
        }
        case Opcode::STORE: {
            auto strParam = reader.readString();
            vm.setVariable(strParam, vm.pop());
            break;
        }
        default: {
            std::cerr << "Invalid opcode " << op << "\n";
            return;
        }
        };
    }
}

int readProgram(Deserializer &reader) {
    size_t methodCount = reader.readInteger();
    if (methodCount == 0) {
        std::cerr << "Error: Program has no methods.";
        return EXIT_FAILURE;
    }
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
    runProgram(reader, vm);
    return 0;
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
    readProgram(reader);
}
