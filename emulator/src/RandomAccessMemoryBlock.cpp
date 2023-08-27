#include "RandomAccessMemoryBlock.h"

namespace lce::Emulator
{
    RandomAccessMemoryBlock::RandomAccessMemoryBlock(uint16_t Size)
    {
        m_Memory.resize(Size, 0);
    }

    uint8_t RandomAccessMemoryBlock::Read(uint16_t RelativeAddress) const
    {
        return m_Memory[RelativeAddress];
    }

    void RandomAccessMemoryBlock::Write(uint16_t RelativeAddress, uint8_t Value)
    {
        m_Memory[RelativeAddress] = Value;
    }

    uint16_t RandomAccessMemoryBlock::Size() const
    {
        return static_cast<uint16_t>(m_Memory.size());
    }
}
