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

    bool CPU::AddMemoryBlock(std::unique_ptr<MemoryBlock> NewBlock, uint16_t StartAddress)
    {
        // Check that there is no overlap with existing memory blocks
        auto EndAddress = StartAddress + NewBlock->Size() - 1; // Last byte that belongs to current memory block
        for (const auto& Block : MemoryBlocks)
        {
            auto CurrentBlockStartAddress = Block.first;
            auto CurrentBlockEndAddress = CurrentBlockStartAddress + Block.second->Size() - 1;

            if (StartAddress >= CurrentBlockStartAddress && StartAddress <= CurrentBlockEndAddress ||
                EndAddress   >= CurrentBlockStartAddress && EndAddress   <= CurrentBlockEndAddress)
            {
                Common::ReportError(Common::ErrorSeverity::Fatal, { __FILE__, 0, __LINE__, 1 }, "Cannot add memory block that overlaps existing ones");
                return false;
            }
        }

        MemoryBlocks.emplace_back(StartAddress, std::move(NewBlock));
        return true;
    }

    uint16_t CPU::GetRegister(Assembler::Register Register) const
    {
        return Registers[static_cast<RegisterIndexUnderlyingType>(Register)];
    }

    MemoryBlock* CPU::FindMemoryBlock(uint16_t AbsoluteAddress, uint16_t& StartAddress) const
    {
        for (const auto& Block : MemoryBlocks)
        {
            if (Block.first <= AbsoluteAddress && Block.first + Block.second->Size() > AbsoluteAddress)
            {
                StartAddress = Block.first;
                return Block.second.get();
            }
        }

        return nullptr;
    }

    uint8_t CPU::ReadByte(uint16_t AbsoluteAddress) const
    {
        uint16_t StartAddress;
        auto* Block = FindMemoryBlock(AbsoluteAddress, StartAddress);

        if (!Block)
        {
            Common::ReportError(Common::ErrorSeverity::Warning, { __FILE__, 0, __LINE__, 0 }, "Reading from invalid memory location 0x{0:X}", AbsoluteAddress);
            return 0;
        }

        return Block->Read(AbsoluteAddress - StartAddress);
    }

    uint16_t CPU::ReadWord(uint16_t AbsoluteAddress) const
    {
        auto Low = ReadByte(AbsoluteAddress);
        auto High = ReadByte(AbsoluteAddress + 1);

        uint16_t Result = (High << 8) | Low;
        return Result;
    }

    void CPU::WriteByte(uint16_t AbsoluteAddress, uint8_t Value)
    {
        uint16_t StartAddress;
        auto* Block = FindMemoryBlock(AbsoluteAddress, StartAddress);

        if (!Block)
        {
            Common::ReportError(Common::ErrorSeverity::Warning, { __FILE__, 0, __LINE__, 0 }, "Writing to invalid memory location 0x{0:X}", AbsoluteAddress);
            return;
        }

        Block->Write(AbsoluteAddress - StartAddress, Value);
    }

    void CPU::WriteWord(uint16_t AbsoluteAddress, uint16_t Value)
    {
        uint8_t Low = static_cast<uint8_t>(Value & 0xFF);
        uint8_t High = static_cast<uint8_t>((Value >> 8) & 0xFF);

        WriteByte(AbsoluteAddress, Low);
        WriteByte(AbsoluteAddress + 1, High);
    }

    void CPU::MovRegImm(std::span<uint8_t> Bytes)
    {
        auto Register = ExtractReg1(Bytes);
        auto Value = ExtractImm(Bytes);

        auto RegisterIndex = static_cast<RegisterIndexUnderlyingType>(Register);
        Registers[RegisterIndex] = Value;
    }
} // namespace lce::Emulator
