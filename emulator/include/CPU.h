#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <vector>

#include "Instruction.h"
#include "MemoryBlock.h"

namespace lce::Emulator
{
    class CPU
    {
    public:
        void Reset();

        void ExecuteSingleInstruction(std::span<uint8_t> Bytes);

        bool AddMemoryBlock(std::unique_ptr<MemoryBlock> NewBlock, uint16_t StartAddress);

        uint16_t GetRegister(Assembler::Register Register) const;

        // NOTE: this should probably be only used in the testing environment
        void SetRegister(Assembler::Register Register, uint16_t Value);

    private:
        using RegisterIndexUnderlyingType = std::underlying_type<Assembler::Register>::type;
        constexpr static size_t RegisterCount = static_cast<RegisterIndexUnderlyingType>(Assembler::Register::Count_);

        uint16_t Registers[RegisterCount];
        std::vector<std::pair<uint16_t, std::unique_ptr<MemoryBlock>>> MemoryBlocks;

        MemoryBlock* FindMemoryBlock(uint16_t AbsoluteAddress, uint16_t& StartAddress) const;

        uint8_t ReadByte(uint16_t AbsoluteAddress) const;
        uint16_t ReadWord(uint16_t AbsoluteAddress) const;

        void WriteByte(uint16_t AbsoluteAddress, uint8_t Value);
        void WriteWord(uint16_t AbsoluteAddress, uint16_t Value);

        uint16_t ReadRegister(Assembler::Register Register) const;
        void WriteRegister(Assembler::Register Register, uint16_t Value);

        void ExecuteMov(const uint8_t* Bytes, const uint8_t* EndOfStream);
    };
} // namespace lce::Emulator
