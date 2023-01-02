#include <gtest/gtest.h>

#include <memory>

#include "CPU.h"
#include "RandomAccessMemoryBlock.h"

using namespace lce::Assembler;
using namespace lce::Emulator;

class TestCPU : public ::testing::Test
{
protected:
    CPU CPU;
    MemoryBlock* Memory = nullptr;

    void SetUp() override
    {
        CPU.Reset();

        auto RAM = std::make_unique<RandomAccessMemoryBlock>(16384);
        Memory = RAM.get();
        CPU.AddMemoryBlock(std::move(RAM), 0x8000);
    }
};

TEST_F(TestCPU, TestReset)
{
    CPU.Reset();

    EXPECT_EQ(CPU.GetRegister(Register::R0), 0);
    EXPECT_EQ(CPU.GetRegister(Register::R1), 0);
    EXPECT_EQ(CPU.GetRegister(Register::R2), 0);
    EXPECT_EQ(CPU.GetRegister(Register::R3), 0);
    EXPECT_EQ(CPU.GetRegister(Register::RSP), 0);
    EXPECT_EQ(CPU.GetRegister(Register::RFL), 0);
}

TEST(TestCPU, TestMOVRegImm)
{
    CPU CPU;
    CPU.Reset();

    uint8_t Instruction[] = { 0b00010000, 0xFE, 0xCA }; // mov R0, 0xCAFE
    CPU.ExecuteSingleInstruction(Instruction);
    EXPECT_EQ(CPU.GetRegister(Register::R0), 0xCAFE);
}
