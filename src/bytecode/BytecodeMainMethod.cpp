#include "bytecode/BytecodeMainMethod.hpp"
#include "semantic/SymbolTable.hpp"
#include <iostream>

void BytecodeMainMethod::print(std::ostream &os) const { block.print(os); }
