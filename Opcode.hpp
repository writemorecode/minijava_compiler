#ifndef OPCODE_HPP
#define OPCODE_HPP

enum Opcode {
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
    STOP = 18
};

#endif
