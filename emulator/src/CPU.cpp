#include "CPU.h"

#include <cassert>
#include <iterator>

#include "ErrorReporting.h"
#include "Instruction.h"

#define TODO()                                                                                     \
    do                                                                                             \
    {                                                                                              \
        Common::ReportError(Common::ErrorSeverity::Fatal, { __FILE__, 0, __LINE__, 1 }, "TODO()"); \
        assert(false);                                                                             \
    } while (0);

namespace lce::Emulator
{
    static constexpr uint8_t EncodedNOP = 0x54;

    enum class OperandType
    {
        Register = 0b00,
        OneByteImmediate = 0b10,
        TwoByteImmediate = 0b11
    };

    static Assembler::Opcode ExtractOpcode(uint8_t FirstByte)
    {
        return static_cast<Assembler::Opcode>((FirstByte >> 2) & 0x1F);
    }

    static OperandType ExtractFirstOperandType(uint8_t FirstByte)
    {
        return static_cast<OperandType>(FirstByte & 0x03);
    }

    static OperandType ExtractSecondOperandType(uint8_t SecondByte)
    {
        return static_cast<OperandType>(SecondByte >> 6);
    }

    static Assembler::Register ExtractFirstRegister(uint8_t SecondByte)
    {
        return static_cast<Assembler::Register>((SecondByte >> 3) & 0x07);
    }

    static Assembler::Register ExtractSecondRegister(uint8_t SecondByte)
    {
        return static_cast<Assembler::Register>(SecondByte & 0x07);
    }

    void CPU::Reset()
    {
        for (auto& Register : m_Registers)
            Register = 0;
    }

    void CPU::ExecuteSingleInstruction(std::span<uint8_t> Bytes)
    {
        assert(Bytes.size() > 0);

        auto Opcode = ExtractOpcode(Bytes[0]);
        switch (Opcode)
        {
        case Assembler::Opcode::Mov:
            ExecuteMov(Bytes.data(), Bytes.data() + Bytes.size_bytes());
            break;
        default:
            TODO();
        }
    }

    void CPU::Run(uint16_t StartAddress)
    {
        m_IP = StartAddress;

        while (!m_IsHalted)
        {
            // NOTE: instructions are at most 4 bytes, so we can fetch 4 bytes or fewer and pass them to the instruction handler
            uint8_t Bytes[4] = { ReadByteOr(m_IP, EncodedNOP), ReadByteOr(m_IP + 1, EncodedNOP), ReadByteOr(m_IP + 2, EncodedNOP), ReadByteOr(m_IP + 3, EncodedNOP) };
            ExecuteSingleInstruction(Bytes);
        }
    }

    bool CPU::AddMemoryBlock(std::unique_ptr<MemoryBlock> NewBlock, uint16_t StartAddress)
    {
        // Check that there is no overlap with existing memory blocks
        auto EndAddress = StartAddress + NewBlock->Size() - 1; // Last byte that belongs to current memory block
        for (const auto& Block : m_MemoryBlocks)
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

        m_MemoryBlocks.emplace_back(StartAddress, std::move(NewBlock));
        return true;
    }

    uint16_t CPU::GetRegister(Assembler::Register Register) const
    {
        return m_Registers[static_cast<RegisterIndexUnderlyingType>(Register)];
    }

    void CPU::SetRegister(Assembler::Register Register, uint16_t Value)
    {
        m_Registers[static_cast<RegisterIndexUnderlyingType>(Register)] = Value;
    }

    std::string CPU::SerializeState() const
    {
        return fmt::format("ip: {:#06x}; r1: {:#06x}; r2: {:#06x}; r3: {:#06x}; rsp: {:#06x}; rfl: {:#06x}; halted: {}",
                           m_IP, ReadRegister(Assembler::Register::R0), ReadRegister(Assembler::Register::R1),
                           ReadRegister(Assembler::Register::R2), ReadRegister(Assembler::Register::R3),
                           ReadRegister(Assembler::Register::RSP), ReadRegister(Assembler::Register::RFL),
                           m_IsHalted ? "true" : "false");
    }

    MemoryBlock* CPU::FindMemoryBlock(uint16_t AbsoluteAddress, uint16_t& StartAddress) const
    {
        for (const auto& Block : m_MemoryBlocks)
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

    uint8_t CPU::ReadByteOr(uint16_t AbsoluteAddress, uint8_t Fallback) const
    {
        uint16_t StartAddress;
        auto* Block = FindMemoryBlock(AbsoluteAddress, StartAddress);

        if (Block)
            return Block->Read(AbsoluteAddress - StartAddress);
        return Fallback;
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


    uint16_t CPU::ReadRegister(Assembler::Register Register) const
    {
        auto RegisterIndex = static_cast<RegisterIndexUnderlyingType>(Register);
        assert(RegisterIndex >= 0 && RegisterIndex < std::size(m_Registers));
        return m_Registers[RegisterIndex];
    }

    void CPU::WriteRegister(Assembler::Register Register, uint16_t Value)
    {
        auto RegisterIndex = static_cast<RegisterIndexUnderlyingType>(Register);
        assert(RegisterIndex >= 0 && RegisterIndex < std::size(m_Registers));
        m_Registers[RegisterIndex] = Value;
    }

    void CPU::ExecuteMov(const uint8_t* Bytes, const uint8_t* EndOfStream)
    {
        assert(Bytes + 1 < EndOfStream); // Mov is at least 2 bytes long

        auto FirstOperandType = ExtractFirstOperandType(Bytes[0]);
        auto SecondOperandType = ExtractSecondOperandType(Bytes[1]);

        uint16_t Value = 0;
        if (SecondOperandType == OperandType::Register)
        {
            auto SourceRegister = ExtractSecondRegister(Bytes[1]);
            Value = ReadRegister(SourceRegister);
        }
        else if (SecondOperandType == OperandType::OneByteImmediate)
        {
            assert(Bytes + 2 < EndOfStream);
            Value = Bytes[2];
        }
        else if (SecondOperandType == OperandType::TwoByteImmediate)
        {
            assert(Bytes + 3 < EndOfStream);
            Value = Bytes[2] | (Bytes[3] << 8);
        }

        assert(FirstOperandType == OperandType::Register);
        auto DestRegister = ExtractFirstRegister(Bytes[1]);

        WriteRegister(DestRegister, Value);
    }
} // namespace lce::Emulator
