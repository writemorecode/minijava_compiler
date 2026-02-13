#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <stack>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "bytecode/Opcode.hpp"
#include "util/serialize.hpp"

struct Instruction {
    Opcode op;
    std::int64_t argNumber;
    std::string argString;
    Instruction(Opcode op_, std::int64_t argNum_, const std::string &argStr_)
        : op(op_), argNumber(argNum_), argString(argStr_) {};
};

[[nodiscard]] std::string
class_name_from_method_name(const std::string &methodName) {
    const auto separator = methodName.find('.');
    if (separator == std::string::npos) {
        return methodName;
    }
    return methodName.substr(0, separator);
}

using Value = std::int64_t;

struct Block {
    std::vector<Instruction> instructions;
    void print() const {
        for (const auto &instr : instructions) {
            const auto opcodeIndex =
                static_cast<size_t>(static_cast<std::uint8_t>(instr.op));
            if (opcodeIndex >= mnemonics.size()) {
                throw std::invalid_argument("invalid opcode in block print");
            }
            std::cout << "\t\t" << mnemonics[opcodeIndex] << "\t";
            if (instr.argString.empty()) {
                std::cout << instr.argNumber << "\n";
            } else {
                std::cout << instr.argString << "\n";
            }
        }
    };
};

struct Method {
    std::vector<std::string> localVariables;
    std::vector<std::string> fieldVariables;
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
    std::unordered_map<std::string, Value> localVariables;
    std::unordered_set<std::string> fieldVariables;
    std::string currentBlock;
    std::string className;
    Value thisReference = 0;

  public:
    explicit Activation(const Method &method_, const std::string &currentBlock_)
        : method(method_), fieldVariables(method_.fieldVariables.begin(),
                                          method_.fieldVariables.end()),
          currentBlock(currentBlock_),
          className(class_name_from_method_name(currentBlock_)) {
        for (const auto &name : method.localVariables) {
            localVariables[name] = 0;
        }
    };
    auto getPC() const { return pc; }
    auto getCurrentBlockName() const { return currentBlock; }
    [[nodiscard]] const auto &getClassName() const { return className; }
    const auto &step() {
        const auto it = method.blocks.find(currentBlock);
        if (it == method.blocks.end()) {
            throw std::invalid_argument("block " + currentBlock + " not found");
        }
        const auto &block = it->second;
        if (pc >= block.instructions.size()) {
            throw std::out_of_range("instruction pointer out of bounds in " +
                                    currentBlock);
        }
        return block.instructions[pc++];
    }
    [[nodiscard]] bool hasLocalVariable(const std::string &name) const {
        return localVariables.contains(name);
    }
    [[nodiscard]] bool hasFieldVariable(const std::string &name) const {
        return fieldVariables.contains(name);
    }
    [[nodiscard]] Value getLocalVariable(const std::string &name) const {
        if (const auto &it = localVariables.find(name);
            it != localVariables.end()) {
            return it->second;
        }
        throw std::invalid_argument("variable " + name + " not found");
    }
    void setLocalVariable(const std::string &name, Value value) {
        localVariables[name] = value;
    }
    [[nodiscard]] Value getThisReference() const { return thisReference; }
    void setThisReference(Value value) { thisReference = value; }
    void setCurrentBlock(const std::string &blockName) {
        currentBlock = blockName;
        pc = 0;
    }
};
class Program {
    std::string mainMethodName;
    Method mainMethod;
    std::unordered_map<std::string, Method> methods;
    std::unordered_map<std::string, std::vector<std::string>> classFieldNames;

    void registerMethodFields(const std::string &methodName,
                              const Method &method) {
        const auto className = class_name_from_method_name(methodName);
        auto &fields = classFieldNames[className];
        for (const auto &fieldName : method.fieldVariables) {
            if (std::find(fields.begin(), fields.end(), fieldName) ==
                fields.end()) {
                fields.push_back(fieldName);
            }
        }
    }

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
          methods(methods_) {
        registerMethodFields(mainMethodName, mainMethod);
        for (const auto &[methodName, method] : methods) {
            registerMethodFields(methodName, method);
        }
    };

    void print() const;
    std::string getMainMethodName() const { return mainMethodName; }
    [[nodiscard]] const std::vector<std::string> &
    getClassFieldNames(const std::string &className) const {
        static const std::vector<std::string> empty;
        if (const auto &it = classFieldNames.find(className);
            it != classFieldNames.end()) {
            return it->second;
        }
        return empty;
    }
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
    struct ObjectInstance {
        std::string className;
        std::unordered_map<std::string, Value> fields;
    };

    std::stack<Value> dataStack;
    std::stack<std::shared_ptr<Activation>> activations;
    std::shared_ptr<Activation> currentActivation;

    Program program;
    std::vector<ObjectInstance> objects;
    std::vector<std::vector<Value>> arrays;

    Instruction step() {
        if (currentActivation == nullptr) {
            std::cerr << "current activation is null!\n";
            std::exit(1);
        }
        return currentActivation->step();
    }

    [[nodiscard]] ObjectInstance &getObjectByReference(Value reference) {
        if (reference <= 0 || static_cast<size_t>(reference) > objects.size()) {
            throw std::invalid_argument("invalid object reference");
        }
        return objects[static_cast<size_t>(reference - 1)];
    }
    [[nodiscard]] const ObjectInstance &
    getObjectByReference(Value reference) const {
        if (reference <= 0 || static_cast<size_t>(reference) > objects.size()) {
            throw std::invalid_argument("invalid object reference");
        }
        return objects[static_cast<size_t>(reference - 1)];
    }

    [[nodiscard]] std::vector<Value> &getArrayByReference(Value reference) {
        if (reference <= 0 || static_cast<size_t>(reference) > arrays.size()) {
            throw std::invalid_argument("invalid array reference");
        }
        return arrays[static_cast<size_t>(reference - 1)];
    }

    [[nodiscard]] Value allocateObject(const std::string &className) {
        ObjectInstance object{.className = className, .fields = {}};
        for (const auto &fieldName : program.getClassFieldNames(className)) {
            if (fieldName == "this") {
                continue;
            }
            object.fields[fieldName] = 0;
        }

        objects.push_back(std::move(object));
        return static_cast<Value>(objects.size());
    }

    void setVariableValue(const std::string &name, Value value) {
        if (currentActivation->hasLocalVariable(name)) {
            currentActivation->setLocalVariable(name, value);
            return;
        }
        if (currentActivation->hasFieldVariable(name)) {
            if (name == "this") {
                currentActivation->setThisReference(value);
                return;
            }
            const auto thisReference = currentActivation->getThisReference();
            if (thisReference == 0) {
                throw std::invalid_argument("this not initialized");
            }
            auto &object = getObjectByReference(thisReference);
            object.fields[name] = value;
            return;
        }
        throw std::invalid_argument("variable " + name + " not found");
    }
    [[nodiscard]] Value getVariableValue(const std::string &name) {
        if (currentActivation->hasLocalVariable(name)) {
            return currentActivation->getLocalVariable(name);
        }
        if (currentActivation->hasFieldVariable(name)) {
            if (name == "this") {
                return currentActivation->getThisReference();
            }
            const auto thisReference = currentActivation->getThisReference();
            if (thisReference == 0) {
                throw std::invalid_argument("this not initialized");
            }
            const auto &object = getObjectByReference(thisReference);
            if (const auto &it = object.fields.find(name);
                it != object.fields.end()) {
                return it->second;
            }
            throw std::invalid_argument("field " + name + " not found");
        }
        throw std::invalid_argument("variable " + name + " not found");
    }

    void push(Value value) { dataStack.push(value); }
    void push(const std::string &name) { push(getVariableValue(name)); }
    Value pop() {
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
        case Opcode::NEW: {
            push(allocateObject(instruction.argString));
            break;
        }
        case Opcode::NEW_ARRAY: {
            const auto length = pop();
            if (length < 0) {
                throw std::invalid_argument("negative array length");
            }
            arrays.emplace_back(length, 0);
            push(static_cast<Value>(arrays.size()));
            break;
        }
        case Opcode::ARRAY_LOAD: {
            const auto index = pop();
            const auto arrayReference = pop();
            const auto &array = getArrayByReference(arrayReference);
            if (index < 0 || static_cast<size_t>(index) >= array.size()) {
                throw std::invalid_argument("array index out of bounds");
            }
            push(array[static_cast<size_t>(index)]);
            break;
        }
        case Opcode::ARRAY_STORE: {
            const auto value = pop();
            const auto index = pop();
            const auto arrayReference = pop();
            auto &array = getArrayByReference(arrayReference);
            if (index < 0 || static_cast<size_t>(index) >= array.size()) {
                throw std::invalid_argument("array index out of bounds");
            }
            array[static_cast<size_t>(index)] = value;
            break;
        }
        case Opcode::ARRAY_LENGTH: {
            const auto arrayReference = pop();
            const auto &array = getArrayByReference(arrayReference);
            push(static_cast<Value>(array.size()));
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
    std::int64_t argNumber = 0;
    std::string argString;
    auto op = reader.readOpcode();
    switch (op) {
    case Opcode::JMP:
    case Opcode::CJMP:
    case Opcode::CALL:
    case Opcode::LOAD:
    case Opcode::STORE:
    case Opcode::NEW: {
        argString = reader.readString();
        break;
    }
    case Opcode::CONST: {
        argNumber = reader.readSignedInteger();
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
    const auto localVariableNames = reader.readStringVector();
    const auto fieldVariableNames = reader.readStringVector();

    const auto blockCount = reader.readInteger();
    // std::cout << "Block count: " << blockCount << "\n";
    std::unordered_map<std::string, Block> blocks;
    for (size_t i = 0; i < blockCount; i++) {
        const auto blockName = reader.readString();
        // std::cout << "Block: " << blockName << "\n";
        blocks.emplace(blockName, readBlock(reader));
    }

    return {localVariableNames, fieldVariableNames, blocks};
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

    std::string filename{argv[1]};
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
