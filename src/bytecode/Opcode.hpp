#ifndef OPCODE_HPP
#define OPCODE_HPP
#include <cstdint>
#include <string>
#include <vector>
enum Opcode : int8_t {
    LOAD = 0,
    CONST = 1,
    STORE = 2,
    ADD = 3,
    SUB = 4,
    MUL = 5,
    DIV = 6,
    LT = 7,
    GT = 8,
    EQ = 9,
    AND = 10,
    OR = 11,
    NOT = 12,
    JMP = 13,
    CJMP = 14,
    CALL = 15,
    RET = 16,
    PRINT = 17,
    STOP = 18,
    NEW = 19,
    NEW_ARRAY = 20,
    ARRAY_LOAD = 21,
    ARRAY_STORE = 22,
    ARRAY_LENGTH = 23
};

const std::vector<std::string> mnemonics{
    "ILOAD", "ICONST", "ISTORE",       "IADD",          "ISUB",    "IMUL",
    "IDIV",  "ILT",    "IGT",          "IEQ",           "IAND",    "IOR",
    "INOT",  "GOTO",   "IFFALSE GOTO", "INVOKEVIRTUAL", "IRETURN", "PRINT",
    "STOP",  "NEW",    "NEWARRAY",     "IALOAD",        "IASTORE", "IALEN"};
#endif
