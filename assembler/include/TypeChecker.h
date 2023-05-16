#pragma once

#include <span>

#include "Instruction.h"

namespace lce::Assembler
{
    bool CheckInstruction(const Instruction& Instruction);

    bool CheckInstructionSequence(std::span<const Instruction> Instructions);
}
