#pragma once

#include <cstdint>
#include <variant>

#include "SourceLocation.h"

namespace lce::Assembler
{
    enum class Opcode
    {
        Mov,
        Lda,
        Sta,
        Add,
        Sub,
        And,
        Or,
        Xor,
        Not,
        Shl,
        Shr,
        Push,
        Pop,
        Jmp,
        Jz,
        Jv,
        Jc,
        Jn,
        Call,
        Ret,
        Nop,
        Hlt
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
