#include "CPU.h"

#include <cassert>

#include "ErrorReporting.h"

#define TODO()                                                                                     \
    do                                                                                             \
    {                                                                                              \
        Common::ReportError(Common::ErrorSeverity::Fatal, { __FILE__, 0, __LINE__, 1 }, "TODO()"); \
        assert(false);                                                                             \
    } while (0);

namespace lce::Emulator
{
    static EncodedOpcode ExtractOpcode(std::span<uint8_t> Bytes)
    {
        assert(Bytes.size() > 0);

        uint32_t Opcode = Bytes[0] >> 3;

        return static_cast<EncodedOpcode>(Opcode);
    }

    // Returns register that is the first argument of the instruction
    static Assembler::Register ExtractReg1(std::span<uint8_t> Bytes)
    {
        assert(Bytes.size() > 0);

        uint8_t Register = Bytes[0] & 0x07;

        return static_cast<Assembler::Register>(Register);
    }

    // Returns immediate value that is the argument of the instruction
    static uint16_t ExtractImm(std::span<uint8_t> Bytes)
    {
        assert(Bytes.size() >= 3);

        auto Low = Bytes[1];
        auto High = Bytes[2];

        uint16_t Result = (High << 8) | Low;

        return Result;
    }

    void CPU::Reset()
    {
        for (auto& Register : Registers)
            Register = 0;
    }

    void CPU::ExecuteSingleInstruction(std::span<uint8_t> Bytes)
    {
        auto Opcode = ExtractOpcode(Bytes);
        switch (Opcode)
        {
        case EncodedOpcode::MovRegImm:
            MovRegImm(Bytes);
            break;
        default:
            TODO();
            break;
        }
    }

    uint16_t CPU::GetRegister(Assembler::Register Register)
    {
        return Registers[static_cast<RegisterIndexUnderlyingType>(Register)];
    }

    void CPU::MovRegImm(std::span<uint8_t> Bytes)
    {
        auto Register = ExtractReg1(Bytes);
        auto Value = ExtractImm(Bytes);

        auto RegisterIndex = static_cast<RegisterIndexUnderlyingType>(Register);
        Registers[RegisterIndex] = Value;
    }
} // namespace lce::Emulator
