#pragma once

#include "SourceLocation.h"

#include <string>
#include <variant>
#include <unordered_map>

namespace lce::Assembler
{
#define ENUMERATE_LEXEM_TYPES(Func)             \
    Func(Undefined)                             \
    Func(EndOfFile)                             \
    Func(LineBreak)                             \
    Func(Identifier)                            \
    Func(NumericLiteral)                        \
    Func(Comma)                                 \
    Func(LeftSquareBracket)                     \
    Func(RightSquareBracket)

    enum class LexemType
    {
#define ENUMERATION_FUNC(Text) Text,
        ENUMERATE_LEXEM_TYPES(ENUMERATION_FUNC)
#undef ENUMERATION_FUNC
    };

    const std::unordered_map<LexemType, const char*> LexemTypeAsString = 
    {
#define ENUMERATION_FUNC(Text) { LexemType::Text, #Text },
        ENUMERATE_LEXEM_TYPES(ENUMERATION_FUNC)
#undef ENUMERATION_FUNC
    };

    struct Lexem
    {
        LexemType Type = LexemType::Undefined;

        Common::SourceLocation Location;

        std::string_view Text;

        std::variant<std::string, uint64_t> ParsedValue;
    };
}
