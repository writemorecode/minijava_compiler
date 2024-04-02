#include "BytecodeMainMethod.hpp"
#include "SymbolTable.hpp"
#include <iostream>

void BytecodeMainMethod::print(std::ostream &os) const { block.print(os); }
