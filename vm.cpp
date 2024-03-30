#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <vector>

constexpr uint64_t MAGIC = 0xBEEFCAFE;

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

    size_t methodCount = 0;
    programFile.read(reinterpret_cast<char *>(&methodCount),
                     sizeof(methodCount));
    if (methodCount == 0) {
        std::cerr << "Error: Program has no methods.";
        return EXIT_FAILURE;
    }
    for (auto i = 0; i < methodCount; i++) {
        size_t methodNameLength = 0;
        programFile.read(reinterpret_cast<char *>(&methodNameLength),
                         sizeof(methodNameLength));
        std::string methodName(methodNameLength, '\0');
        programFile.read(&methodName[0], methodNameLength);
    }
}
