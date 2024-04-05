#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>

std::streamsize readIntegerFromStream(std::ifstream &stream) {
    std::streamsize value = 0;
    stream.read(reinterpret_cast<char *>(&value), sizeof(value));
    return value;
}

std::string readStringFromStream(std::ifstream &stream) {
    auto length = readIntegerFromStream(stream);
    std::string string(length, '\0');
    stream.read(string.data(), length);
    return string;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [bytecode program]\n";
        return EXIT_FAILURE;
    }

    const std::string inputFilename{argv[1]};
    std::ifstream programFile(inputFilename, std::ios::binary);
    if (programFile.eof() || programFile.eof()) {
        std::cerr << "Error: Unable to open file " << inputFilename << ".\n";
        return EXIT_FAILURE;
    }

    size_t methodCount = readIntegerFromStream(programFile);
    if (methodCount == 0) {
        std::cerr << "Error: Program has no methods.";
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < methodCount; i++) {
        auto methodName = readStringFromStream(programFile);
        std::cout << std::quoted(methodName) << "\n";

        size_t variableCount = readIntegerFromStream(programFile);
        for (size_t i = 0; i < variableCount; i++) {
            auto variableName = readStringFromStream(programFile);
            std::cout << "\t" << std::quoted(variableName) << "\n";
        }
    }
}
