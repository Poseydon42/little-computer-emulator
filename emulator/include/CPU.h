#pragma once

#include <cstddef>
#include <span>

#include "Instruction.h"

namespace lce::Emulator
{
    enum class EncodedOpcode : uint32_t
    {
        Hlt = 0b00000,

        MovRegReg = 0b00001, // Rx1 <- Rx2
        MovRegImm = 0b00010, // Rx1 <- Imm
        MovRegImmAddr = 0b00011, // Rx1 <- [Imm]
        MovRegRegAddr = 0b00100, // Rx1 <- [Rx2]
        MovImmAddrReg = 0b00101, // [Imm] <- Rx1
        MovRegAddrReg = 0b00110, // [Rx1] <- Rx2

        AddRegReg = 0b00111, // Rx1 <- Rx1 + Rx2
        AddRegImm = 0b01000, // Rx1 <- Rx1 + Imm

        SubRegReg = 0b01001, // Rx1 <- Rx1 - Rx2
        SubRegImm = 0b01010, // Rx1 <- Rx1 - Imm

        AndRegReg = 0b01011, // Rx1 <- Rx1 AND Rx2
        AndRegImm = 0b01100, // Rx1 <- Rx1 AND Imm

        OrRegReg = 0b01101, // Rx1 <- Rx1 OR Rx2
        OrRegImm = 0b01110, // Rx1 <- Rx1 OR Imm

        XorRegReg = 0b01111, // Rx1 <- Rx1 XOR Rx2
        XorRegImm = 0b10000, // Rx1 <- Rx1 XOR Imm

        NotReg = 0b10001, // Rx1 <- NOT Rx1

        PushReg = 0b10010, // [RSP] <- Rx1; RSP <- RSP + 2
        PushImm = 0b10011, // [RSP] <- Imm; RSP <- RSP + 2

        PopReg = 0b10100, // Rx1 <- [RSP - 2]; RSP <- RSP - 2

        JmpImm = 0b10101, // RIP <- Imm
        JmpReg = 0b10110, // RIP <- Rx1

        JzImm = 0b10111, // if (zero is set) => RIP <- Imm
        JzReg = 0b11000, // if (zero is set) => RIP <- Rx1

        JgImm = 0b11001, // if (overflow is set) => RIP <- Imm
        JgReg = 0b11010, // if (overflow is set) => RIP <- Rx1

        CallImm = 0b11011, // [RSP] <- RIP; RSP <- RSP + 2; RIP <- Imm
        CallReg = 0b11100, // [RSP] <- RIP; RSP <- RSP + 2; RIP <- Rx1

        Ret = 0b11101, // RIP <- [RSP - 2]; RSP <- RSP - 2
    };

    class CPU
    {
    public:
        void Reset();

        void ExecuteSingleInstruction(std::span<uint8_t> Bytes);

        uint16_t GetRegister(Assembler::Register Register);

    private:
        using RegisterIndexUnderlyingType = std::underlying_type<Assembler::Register>::type;
        constexpr static size_t RegisterCount = static_cast<RegisterIndexUnderlyingType>(Assembler::Register::Count_);

        uint16_t Registers[RegisterCount];

        void MovRegImm(std::span<uint8_t> Bytes);
    };
} // namespace lce::Emulator
