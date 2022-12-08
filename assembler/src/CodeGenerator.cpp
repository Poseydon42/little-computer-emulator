#include "CodeGenerator.h"

#include <cassert>
#include <unordered_map>

#include "ErrorReporting.h"

#define FIXME()                                                                                            \
    {                                                                                                      \
        Common::ReportError(Common::ErrorSeverity::Fatal, {}, "FIXME at {}(line {})", __FILE__, __LINE__); \
        assert(false);                                                                                     \
    }

namespace lce::Assembler
{
    static void MovGenerator(const Instruction& Instruction, std::vector<uint8_t>& Destination)
    {
        if (Instruction.Operands[0].Type == OperandType::Register && Instruction.Operands[1].Type == OperandType::Immediate)
        {
            static constexpr uint8_t OpcodeMask = 0b00010000;

            uint8_t OpcodeByte = OpcodeMask | static_cast<uint8_t>(std::get<Register>(Instruction.Operands[0].Value));
            
            uint8_t LowImmediateByte = static_cast<uint8_t>(std::get<uint64_t>(Instruction.Operands[1].Value) & 0xFF);
            uint8_t HighImmediateByte = static_cast<uint8_t>((std::get<uint64_t>(Instruction.Operands[1].Value) >> 8) & 0xFF);

            Destination.push_back(OpcodeByte);
            Destination.push_back(LowImmediateByte);
            Destination.push_back(HighImmediateByte);
        }
        else
        {
            FIXME();
        }
    }

    static void GenerateMachineCodeForInstruction(const Instruction& Instruction, std::vector<uint8_t>& Destination)
    {
        using InstructionGeneratorFuncType = void (*)(const struct Instruction&, std::vector<uint8_t>&);
        static const std::unordered_map<Opcode, InstructionGeneratorFuncType> InstructionGenerators = {
            { Opcode::Mov, MovGenerator }
        };

        if (!InstructionGenerators.contains(Instruction.Opcode))
        {
            Common::ReportError(Common::ErrorSeverity::Fatal, {}, "Unimplemented generator for opcode");
            assert(false);
        }

        auto Generator = InstructionGenerators.at(Instruction.Opcode);
        Generator(Instruction, Destination);
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
