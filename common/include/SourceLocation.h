#pragma once

#include <string_view>
#include <cstddef>

namespace lce::Assembler
{
    struct SourceLocation
    {
        std::string_view FileName;

        size_t LinearOffset = 0;
        
        size_t Line = 1;
        size_t Column = 1;
    };
}
