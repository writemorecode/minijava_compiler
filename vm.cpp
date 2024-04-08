#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string>

#include "serialize.hpp"

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

    Deserializer reader(programFile);

    size_t methodCount = reader.readInteger();
    if (methodCount == 0) {
        std::cerr << "Error: Program has no methods.";
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < methodCount; i++) {
        auto methodName = reader.readString();
        std::cout << std::quoted(methodName) << "\n";

        const auto variables = reader.readStringVector();
        for (const auto &var : variables) {
            std::cout << "\t" << std::quoted(var) << "\n";
        }
    }
}
