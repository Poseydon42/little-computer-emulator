#pragma once

#include <cstdint>

namespace lce::Emulator
{
    class MemoryBlock
    {
    public:
        virtual ~MemoryBlock() = default;

        virtual uint8_t Read(uint16_t RelativeAddress) const = 0;

        virtual void Write(uint16_t RelativeAddress, uint8_t Value) = 0;

        virtual uint16_t Size() const = 0;
    };
} // namespace lce::Emulator
