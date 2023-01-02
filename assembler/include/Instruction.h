#pragma once

#include <cstdint>
#include <variant>

namespace lce::Assembler
{
    enum class Opcode
    {
        Mov,
        Add,
        Sub,
        And,
        Or,
        Xor,
        Not,
        Push,
        Pop,
        Jmp,
        Jz,
        Jg,
        Call,
        Ret,
        Hlt
    };

    enum class OperandType
    {
        None,

        Immediate,
        Register,
        ImmediateAddress,
        RegisterAddress
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
    };
} // namespace lce::Assembler
