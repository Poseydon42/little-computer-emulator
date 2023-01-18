#pragma once

#include <optional>
#include <span>
#include <vector>

#include "Instruction.h"
#include "Lexer.h"

namespace lce::Assembler
{
    std::optional<Instruction> ParseInstruction(std::span<Lexem> Lexems);

    bool Parse(Lexer& Lexer, std::vector<Instruction>& Destination);
} // namespace lce::Assembler
