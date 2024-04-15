#include <cstdlib>
#include <exception>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "Opcode.hpp"
#include "serialize.hpp"

struct Instruction {
    Opcode op;
    size_t argNumber;
    std::string argString;
    Instruction(Opcode op_, size_t argNum_, const std::string &argStr_)
        : op(op_), argNumber(argNum_), argString(argStr_){};
};

struct Method {
    std::vector<std::string> variables;
    std::vector<Instruction> instructions;
};

class Activation {
    size_t pc = 0;
    std::unordered_map<std::string, size_t> variables;
    std::vector<Instruction> instructions;

  public:
    explicit Activation(const Method &method)
        : instructions{method.instructions} {
        for (const auto &name : method.variables) {
            variables.emplace(name, 0);
        }
    };

    const auto &step() { return instructions[pc++]; }
    void addInstruction(const auto &instr) { instructions.push_back(instr); }
    void setVariable(const std::string &name, size_t value) {
        variables[name] = value;
    }
    void addVariable(const std::string &name) { variables.emplace(name, 0); }
    size_t getVariable(const std::string &name) const {
        if (const auto &it = variables.find(name); it != variables.end()) {
            return it->second;
        }
        // FIXME: Find a way to remove this exception
        throw std::invalid_argument("variable not found");
    }
};
class Program {
    Method mainMethod;
    std::unordered_map<std::string, Method> methods;

  public:
    const auto &getMain() const { return mainMethod; }
    [[nodiscard]] std::shared_ptr<Activation>
    getMethodActivation(const std::string &name) const {
        const auto &it = methods.find(name);
        return std::make_shared<Activation>(it->second);
    }
    Program(const Method &mainMethod_,
            const std::unordered_map<std::string, Method> &methods_)
        : mainMethod(mainMethod_), methods(methods_){};
};

class VM {
    std::stack<size_t> dataStack;
    std::stack<std::shared_ptr<Activation>> activations;
    std::shared_ptr<Activation> currentActivation;

    Program program;

    Instruction step() { return currentActivation->step(); }
    void addInstruction(const auto &instr) {
        currentActivation->addInstruction(instr);
    }
    void addVariable(const std::string &name) {
        currentActivation->addVariable(name);
    }
    void setVariableValue(const std::string &name, size_t value) {
        currentActivation->setVariable(name, value);
    }
    size_t getVariableValue(const std::string &name) const {
        return currentActivation->getVariable(name);
    }

    void push(size_t value) { dataStack.push(value); }
    void push(const std::string &name) { push(getVariableValue(name)); }
    size_t pop() {
        if (dataStack.empty()) {
            throw std::runtime_error("empty data stack");
        }
        auto value = dataStack.top();
        dataStack.pop();
        return value;
    }
    bool isEmpty() { return dataStack.empty(); }

    void pushCurrentActivation() { activations.push(currentActivation); }

    void setNewActivation(const std::string &name) {
        currentActivation = program.getMethodActivation(name);
    }

  public:
    explicit VM(const Program &program_) : program{program_} {
        currentActivation = std::make_shared<Activation>(program_.getMain());
    };
    void run();
};

void VM::run() {
    while (true) {
        const auto instruction = step();
        const auto op = instruction.op;
        switch (op) {
        case Opcode::STOP: {
            std::cout << "Reached end of program.\n";
            return;
        }
        case Opcode::RET: {
            currentActivation.swap(activations.top());
            activations.pop();
            std::cout << "Returning from method.\n";
            break;
        }
        case Opcode::CALL: {
            activations.push(currentActivation);
            currentActivation =
                program.getMethodActivation(instruction.argString);
            std::cout << "Calling method " << instruction.argString << "\n";
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
            setVariableValue(instruction.argString, pop());
            break;
        }
        default: {
            std::cerr << "Invalid instruction: ";
            std::cerr << mnemonics[op] << " " << instruction.argString << "\n";
            return;
        }
        };
    }
}

[[nodiscard]] Instruction readInstruction(Deserializer &reader) {
    size_t argNumber = 0;
    std::string argString;
    auto op = reader.readOpcode();
    switch (op) {
    case Opcode::JMP:
    case Opcode::CJMP:
    case Opcode::CALL:
    case Opcode::LOAD:
    case Opcode::STORE: {
        argString = reader.readString();
        break;
    }
    case Opcode::CONST: {
        argNumber = reader.readInteger();
        break;
    }
    default: {
        break;
    }
    }

    return {op, argNumber, argString};
}

[[nodiscard]] Method readMethod(Deserializer &reader) {
    Method method;
    const auto variableNames = reader.readStringVector();
    for (const auto &name : variableNames) {
        method.variables.push_back(name);
    }
    const auto instructionCount = reader.readInteger();
    std::cout << "Instruction count: " << instructionCount << "\n";
    method.instructions.reserve(instructionCount);
    for (size_t i = 0; i < instructionCount; i++) {
        method.instructions.emplace_back(readInstruction(reader));
    }
    return method;
}

[[nodiscard]] Program readProgram(Deserializer &reader) {
    const auto mainMethodName = reader.readString();
    std::cout << "Method: " << std::quoted(mainMethodName) << "\n";
    const auto mainMethod = readMethod(reader);

    std::unordered_map<std::string, Method> methods;
    const auto methodCount = reader.readInteger();
    for (size_t i = 0; i < methodCount; i++) {
        const auto methodName = reader.readString();
        const auto method = readMethod(reader);
        methods.emplace(methodName, method);
    }
    return {mainMethod, methods};
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
    Program program = readProgram(reader);
    VM vm(program);

    try {
        vm.run();
    } catch (const std::exception &exc) {
        std::cerr << "VM threw exception: " << exc.what() << "\n";
    };
}
