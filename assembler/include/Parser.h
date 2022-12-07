#pragma once

#include <vector>

#include "Instruction.h"
#include "Lexer.h"

namespace lce::Assembler
{
    bool Parse(Lexer& Lexer, std::vector<Instruction>& Destination);
} // namespace lce::Assembler
