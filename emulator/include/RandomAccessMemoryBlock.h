#pragma once

#include <cstdint>
#include <vector>

#include "MemoryBlock.h"

namespace lce::Emulator
{
    class RandomAccessMemoryBlock : public MemoryBlock
    {
    public:
        RandomAccessMemoryBlock(uint16_t Size);

        virtual uint8_t Read(uint16_t RelativeAddress) const override;

        virtual void Write(uint16_t RelativeAddress, uint8_t Value) override;

        virtual uint16_t Size() const override;

    private:
        std::vector<uint8_t> Memory;
    };
}
