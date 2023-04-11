#include "CodeGenerator.h"

#include <cassert>
#include <cstdint>
#include <limits>
#include <unordered_map>

#include "ErrorReporting.h"
#include "Instruction.h"

namespace lce::Assembler
{
    static constexpr auto OpcodeLength = 6;
    static constexpr auto OpcodeShift = 8 - OpcodeLength;

    static constexpr auto FirstOperandTypeShift = 0;
    static constexpr auto SecondOperandTypeShift = 6;

    static constexpr auto FirstRegisterOperandShift = 3;
    static constexpr auto SecondRegisterOperandShift = 0;

    static constexpr uint8_t OperandTypeRegister = 0b00;
    static constexpr uint8_t OperandTypeOneByteImmediate = 0b10;
    static constexpr uint8_t OperandTypeTwoByteImmediate = 0b11;

    static std::pair<uint8_t, uint8_t> ExtractImmediate(const Instruction& Instruction, size_t OperandIndex)
    {
        assert(OperandIndex < 2);

        auto RawValue = std::get<uint64_t>(Instruction.Operands[OperandIndex].Value);
        if (RawValue > std::numeric_limits<uint16_t>::max())
            Common::ReportError(Common::ErrorSeverity::Warning, Instruction.Location, "Immediate argument value {} exceeds 16 bits; a truncated version will be written", RawValue);

        auto MostSignificanByte = static_cast<uint8_t>(RawValue >> 8);
        auto LeastSignificantByte = static_cast<uint8_t>(RawValue >> 0);

        return std::make_pair(MostSignificanByte, LeastSignificantByte);
    }

    static void GenerateMachineCodeForInstruction(const Instruction& Instruction, std::vector<uint8_t>& Destination)
    {
        uint8_t Opcode = static_cast<uint8_t>(Instruction.Opcode);

        if (Instruction.Operands[0].Type == OperandType::None && Instruction.Operands[1].Type == OperandType::None)
        {
            uint8_t FirstByte = Opcode << OpcodeShift;
            Destination.push_back(FirstByte);
        }
        else if (Instruction.Operands[0].Type == OperandType::Register && Instruction.Operands[1].Type == OperandType::None)
        {
            auto RegisterIndex = static_cast<uint8_t>(std::get<Register>(Instruction.Operands[0].Value));

            uint8_t FirstByte = (Opcode << OpcodeShift) | (OperandTypeRegister << FirstOperandTypeShift);
            uint8_t SecondByte = RegisterIndex << FirstRegisterOperandShift;

            Destination.push_back(FirstByte);
            Destination.push_back(SecondByte);
        }
        else if (Instruction.Operands[0].Type == OperandType::Immediate && Instruction.Operands[1].Type == OperandType::None)
        {
            auto [MSB, LSB] = ExtractImmediate(Instruction, 0);
            
            uint8_t FirstOperandTypeBitcode;
            if (MSB > 0)
                FirstOperandTypeBitcode = OperandTypeTwoByteImmediate;
            else
                FirstOperandTypeBitcode = OperandTypeOneByteImmediate;

            uint8_t FirstByte = (Opcode << OpcodeShift) | (FirstOperandTypeBitcode << FirstOperandTypeShift);
            Destination.push_back(FirstByte);

            if (FirstOperandTypeBitcode == OperandTypeOneByteImmediate)
            {
                uint8_t SecondByte = static_cast<uint8_t>(LSB);
                Destination.push_back(SecondByte);
            }
            else
            {
                uint8_t SecondByte = static_cast<uint8_t>(LSB);
                uint8_t ThirdByte = static_cast<uint8_t>(MSB);
                Destination.push_back(SecondByte);
                Destination.push_back(ThirdByte);
            }
        }
        else if (Instruction.Operands[0].Type == OperandType::Register && Instruction.Operands[1].Type == OperandType::Register)
        {
            auto FirstRegister = std::get<Register>(Instruction.Operands[0].Value);
            auto SecondRegister = std::get<Register>(Instruction.Operands[1].Value);

            auto FirstRegisterBits = static_cast<uint8_t>(FirstRegister);
            auto SecondRegisterBits = static_cast<uint8_t>(SecondRegister);

            uint8_t FirstByte = (Opcode << OpcodeShift) | (OperandTypeRegister << FirstOperandTypeShift);
            uint8_t SecondByte = (OperandTypeRegister << SecondOperandTypeShift) | (FirstRegisterBits << FirstRegisterOperandShift) | (SecondRegisterBits << SecondRegisterOperandShift);

            Destination.push_back(FirstByte);
            Destination.push_back(SecondByte);
        }
        else if (Instruction.Operands[0].Type == OperandType::Register && Instruction.Operands[1].Type == OperandType::Immediate)
        {
            auto RegisterBits = static_cast<uint8_t>(std::get<Register>(Instruction.Operands[0].Value));

            uint8_t FirstByte = (Opcode << OpcodeShift) | (OperandTypeRegister << FirstOperandTypeShift);
            Destination.push_back(FirstByte);

            auto [MSB, LSB] = ExtractImmediate(Instruction, 1);
            if (MSB > 0)
            {
                uint8_t SecondByte = (OperandTypeTwoByteImmediate << SecondOperandTypeShift) | (RegisterBits << FirstRegisterOperandShift);
                uint8_t ThirdByte = LSB;
                uint8_t FourthByte = MSB;

                Destination.push_back(SecondByte);
                Destination.push_back(ThirdByte);
                Destination.push_back(FourthByte);
            }
            else
            {
                uint8_t SecondByte = (OperandTypeOneByteImmediate << SecondOperandTypeShift) | (RegisterBits << FirstRegisterOperandShift);
                uint8_t ThirdByte = LSB;

                Destination.push_back(SecondByte);
                Destination.push_back(ThirdByte);
            }
        }
        else if (Instruction.Operands[0].Type == OperandType::Immediate && Instruction.Operands[1].Type == OperandType::Register)
        {
            auto [MSB, LSB] = ExtractImmediate(Instruction, 0);
            auto RegisterBits = static_cast<uint8_t>(std::get<Register>(Instruction.Operands[1].Value));

            if (MSB > 0)
            {
                uint8_t FirstByte = (Opcode << OpcodeShift) | (OperandTypeTwoByteImmediate << FirstOperandTypeShift);
                uint8_t SecondByte = (OperandTypeRegister << SecondOperandTypeShift) | (RegisterBits << SecondRegisterOperandShift);

                Destination.push_back(FirstByte);
                Destination.push_back(SecondByte);
                Destination.push_back(LSB);
                Destination.push_back(MSB);
            }
            else
            {
                uint8_t FirstByte = (Opcode << OpcodeShift) | (OperandTypeOneByteImmediate << FirstOperandTypeShift);
                uint8_t SecondByte = (OperandTypeRegister << SecondOperandTypeShift) | (RegisterBits << SecondRegisterOperandShift);

                Destination.push_back(FirstByte);
                Destination.push_back(SecondByte);
                Destination.push_back(LSB);
            }
        }
        else
        {
            // NOTE: any other combination is invalid
            assert(false);
        }
    }

    std::vector<uint8_t> GenerateMachineCode(const std::vector<Instruction>& Insturctions)
    {
        std::vector<uint8_t> Result;

        for (const auto& Instruction : Insturctions)
        {
            GenerateMachineCodeForInstruction(Instruction, Result);
        }

        return Result;
    }
} // namespace lce::Assembler
