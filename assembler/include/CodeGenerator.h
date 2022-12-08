#pragma once

#include <cstdint>
#include <vector>

#include "Instruction.h"

namespace lce::Assembler
{
    std::vector<uint8_t> GenerateMachineCode(const std::vector<Instruction>& Instructions);
}
