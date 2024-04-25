#include <cstdlib>
#include <fstream>
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

struct Block {
    std::vector<Instruction> instructions;
    void print() const {
        for (const auto &instr : instructions) {
            std::cout << "\t\t" << mnemonics[instr.op] << "\t";
            if (instr.argString.empty()) {
                std::cout << instr.argNumber << "\n";
            } else {
                std::cout << instr.argString << "\n";
            }
        }
    };
};

struct Method {
    std::vector<std::string> variables;
    std::unordered_map<std::string, Block> blocks;

    void print() const {
        for (const auto &[name, block] : blocks) {
            std::cout << "\tblock " << name << "\n";
            block.print();
        }
    };
};

class Activation {
    size_t pc = 0;
    Method method;
    std::unordered_map<std::string, size_t> variables;
    std::string currentBlock;

  public:
    explicit Activation(const Method &method_, const std::string &currentBlock_)
        : method(method_), currentBlock(currentBlock_) {
        for (const auto &name : method.variables) {
            variables[name] = 0;
        }
    };
    auto getPC() const { return pc; }
    auto getCurrentBlockName() const { return currentBlock; }
    const auto &step() {
        const auto &block = method.blocks[currentBlock];
        return block.instructions[pc++];
    }
    void setVariable(const std::string &name, size_t value) {
        variables[name] = value;
    }
    [[nodiscard]] size_t getVariable(const std::string &name) const {
        if (const auto &it = variables.find(name); it != variables.end()) {
            return it->second;
        }
        // FIXME: Find a way to remove this exception
        throw std::invalid_argument("variable " + name + " not found");
    }
    void setCurrentBlock(const std::string &blockName) {
        currentBlock = blockName;
        pc = 0;
    }
};
class Program {
    std::string mainMethodName;
    Method mainMethod;
    std::unordered_map<std::string, Method> methods;

  public:
    const auto &getMain() const { return mainMethod; }
    [[nodiscard]] std::shared_ptr<Activation>
    getMethodActivation(const std::string &name) const {
        const auto &it = methods.find(name);
        if (it == methods.end()) {
            std::cerr << "no such method " << name << "\n";
            return nullptr;
        }
        return std::make_shared<Activation>(it->second, it->first);
    }
    Program(const std::string &mainMethodName_, const Method &mainMethod_,
            const std::unordered_map<std::string, Method> &methods_)
        : mainMethodName(mainMethodName_), mainMethod(mainMethod_),
          methods(methods_){};

    void print() const;
    std::string getMainMethodName() const { return mainMethodName; }
};

void Program::print() const {
    std::cout << "method " << mainMethodName << "\n";
    mainMethod.print();
    for (const auto &[name, method] : methods) {
        std::cout << "method " << name << "\n";
        method.print();
    }
}

class VM {
    std::stack<size_t> dataStack;
    std::stack<std::shared_ptr<Activation>> activations;
    std::shared_ptr<Activation> currentActivation;

    Program program;

    Instruction step() {
        if (currentActivation == nullptr) {
            std::cerr << "current activation is null!\n";
            std::exit(1);
        }
        return currentActivation->step();
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
        currentActivation = std::make_shared<Activation>(
            program_.getMain(), program.getMainMethodName());
    };
    void run();

    void printstack() {
        std::cout << "stack top: ";
        if (!dataStack.empty()) {
            std::cout << dataStack.top() << '\n';
        }
    }
};

void VM::run() {
    while (true) {
        const auto instruction = step();
        const auto op = instruction.op;

        switch (op) {
        case Opcode::STOP: {
            // std::cout << "Reached end of program.\n";
            return;
        }
        case Opcode::RET: {
            currentActivation.swap(activations.top());
            activations.pop();
            // std::cout << "Returning from method.\n";
            break;
        }
        case Opcode::CALL: {
            activations.push(currentActivation);
            currentActivation =
                program.getMethodActivation(instruction.argString);
            if (currentActivation == nullptr) {
                std::cerr << "error: no activation found for method "
                          << instruction.argString << "\n";
                return;
            }
            // std::cout << "Calling method " << instruction.argString << "\n";
            break;
        }
        case Opcode::JMP: {
            currentActivation->setCurrentBlock(instruction.argString);
            break;
        }
        case Opcode::CJMP: {
            const auto conditionValue = pop();
            if (conditionValue == 0) {
                currentActivation->setCurrentBlock(instruction.argString);
            }

            break;
        }
        case Opcode::PRINT: {
            const auto value = pop();
            std::cout << value << "\n";
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
            std::cerr << "Invalid opcode: " << op << "\n";
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

[[nodiscard]] Block readBlock(Deserializer &reader) {
    const auto instructionCount = reader.readInteger();
    // std::cout << "Instruction count: " << instructionCount << "\n";
    std::vector<Instruction> instructions;
    instructions.reserve(instructionCount);
    for (size_t i = 0; i < instructionCount; i++) {
        instructions.emplace_back(readInstruction(reader));
    }
    return {instructions};
}

[[nodiscard]] Method readMethod(Deserializer &reader) {
    const auto variableNames = reader.readStringVector();

    const auto blockCount = reader.readInteger();
    // std::cout << "Block count: " << blockCount << "\n";
    std::unordered_map<std::string, Block> blocks;
    for (size_t i = 0; i < blockCount; i++) {
        const auto blockName = reader.readString();
        // std::cout << "Block: " << blockName << "\n";
        blocks.emplace(blockName, readBlock(reader));
    }

    return {variableNames, blocks};
}

[[nodiscard]] Program readProgram(Deserializer &reader) {
    const auto mainMethodName = reader.readString();
    // std::cout << "Method: " << std::quoted(mainMethodName) << "\n";
    const auto mainMethod = readMethod(reader);

    std::unordered_map<std::string, Method> methods;
    const auto methodCount = reader.readInteger();
    for (size_t i = 0; i < methodCount; i++) {
        const auto methodName = reader.readString();
        // std::cout << "Method: " << std::quoted(methodName) << "\n";
        const auto method = readMethod(reader);
        methods.emplace(methodName, method);
    }
    return {mainMethodName, mainMethod, methods};
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
