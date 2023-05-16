#include "TypeChecker.h"

#include <unordered_set>

#include "ErrorReporting.h"
#include "Instruction.h"

namespace lce::Assembler
{
    struct InstructionOperandCombination
    {
        Opcode Opcode;
        OperandType Operands[2];

        bool operator==(const InstructionOperandCombination& Other) const
        {
            return Opcode == Other.Opcode && Operands[0] == Other.Operands[0] && Operands[1] == Other.Operands[1];
        }
    };

    struct HashInstructionOperandCombination
    {
        size_t operator()(const InstructionOperandCombination& InstructionOperandCombination) const
        {
            return (static_cast<int32_t>(InstructionOperandCombination.Opcode) ^
                    static_cast<int32_t>(InstructionOperandCombination.Operands[0]) ^
                    static_cast<int32_t>(InstructionOperandCombination.Operands[1]));
        }
    };

    bool CheckInstruction(const Instruction& Instruction)
    {
        static const std::unordered_set<InstructionOperandCombination, HashInstructionOperandCombination> AllowedCombinations = {
            { Opcode::Mov, { OperandType::Register, OperandType::Register } },
            { Opcode::Mov, { OperandType::Register, OperandType::Immediate } },

            { Opcode::Lda, { OperandType::Register, OperandType::Immediate } },
            { Opcode::Lda, { OperandType::Register, OperandType::Register } },
            
            { Opcode::Sta, { OperandType::Immediate, OperandType::Register } },
            { Opcode::Sta, { OperandType::Register, OperandType::Register } },

            { Opcode::Add, { OperandType::Register, OperandType::Register } },
            { Opcode::Add, { OperandType::Register, OperandType::Immediate } },

            { Opcode::Sub, { OperandType::Register, OperandType::Register } },
            { Opcode::Sub, { OperandType::Register, OperandType::Immediate } },

            { Opcode::And, { OperandType::Register, OperandType::Register } },
            { Opcode::And, { OperandType::Register, OperandType::Immediate } },

            { Opcode::Or, { OperandType::Register, OperandType::Register } },
            { Opcode::Or, { OperandType::Register, OperandType::Immediate } },

            { Opcode::Xor, { OperandType::Register, OperandType::Register } },
            { Opcode::Xor, { OperandType::Register, OperandType::Immediate } },

            { Opcode::Not, { OperandType::Register, OperandType::None } },

            { Opcode::Push, { OperandType::Register, OperandType::None } },
            { Opcode::Push, { OperandType::Immediate, OperandType::None } },

            { Opcode::Pop, { OperandType::Register, OperandType::None } },

            { Opcode::Jmp, { OperandType::Immediate, OperandType::None } },
            { Opcode::Jmp, { OperandType::Register, OperandType::None } },

            { Opcode::Jz, { OperandType::Immediate, OperandType::None } },
            { Opcode::Jz, { OperandType::Register, OperandType::None } },

            { Opcode::Jv, { OperandType::Immediate, OperandType::None } },
            { Opcode::Jv, { OperandType::Register, OperandType::None } },

            { Opcode::Jc, { OperandType::Immediate, OperandType::None } },
            { Opcode::Jc, { OperandType::Register, OperandType::None } },

            { Opcode::Jn, { OperandType::Immediate, OperandType::None } },
            { Opcode::Jn, { OperandType::Register, OperandType::None } },

            { Opcode::Call, { OperandType::Immediate, OperandType::None } },
            { Opcode::Call, { OperandType::Register, OperandType::None } },

            { Opcode::Ret, { OperandType::None, OperandType::None } },

            { Opcode::Nop, { OperandType::None, OperandType::None } },

            { Opcode::Hlt, { OperandType::None, OperandType::None } }
        };

        InstructionOperandCombination Combination = { Instruction.Opcode, { Instruction.Operands[0].Type, Instruction.Operands[1].Type } };
        
        return AllowedCombinations.contains(Combination);
    }

    bool CheckInstructionSequence(std::span<const Instruction> Instructions)
    {
        bool Result = true;
        
        for (const auto& Instruction : Instructions)
        {
            if (!CheckInstruction(Instruction))
            {
                Common::ReportError(Common::ErrorSeverity::Error, Instruction.Location, "Invalid instruction arguments combination");
                Result = false;
            }
        }

        return Result;
    }
}
