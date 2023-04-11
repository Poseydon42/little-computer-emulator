#pragma once

#include <cstdint>
#include <variant>

#include "SourceLocation.h"

namespace lce::Assembler
{
    enum class Opcode
    {
        Mov = 0b000001,
        Lda = 0b000010,
        Sta = 0b000011,
        Add = 0b000100,
        Sub = 0b000101,
        And = 0b000110,
        Or  = 0b000111,
        Xor = 0b001000,
        Not = 0b001001,
        Shl = 0b001010,
        Shr = 0b001011,
        Push = 0b001100,
        Pop = 0b001101,
        Jmp = 0b001110,
        Jz  = 0b001111,
        Jv  = 0b010000,
        Jc  = 0b010001,
        Jn  = 0b010010,
        Call = 0b010011,
        Ret = 0b010100,
        Nop = 0b010101,
        Hlt = 0b000000
    };

    enum class OperandType
    {
        None,

        Immediate,
        Register,
    };

    enum class Register : uint8_t
    {
        R0 = 0b000,
        R1 = 0b001,
        R2 = 0b010,
        R3 = 0b011,

        RSP = 0b100,
        RFL = 0b101,

        Count_ = 0b1000
    };

    struct Operand
    {
        OperandType Type = OperandType::None;
        std::variant<uint64_t, Register> Value;
    };

    struct Instruction
    {
        Opcode Opcode = Opcode::Hlt;

        Operand Operands[2];

        Common::SourceLocation Location;
    };
} // namespace lce::Assembler
