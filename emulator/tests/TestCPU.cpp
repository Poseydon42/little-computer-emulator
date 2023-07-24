#include <gtest/gtest.h>

#include <memory>

#include "CPU.h"
#include "Instruction.h"
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

TEST_F(TestCPU, TestMovRegReg)
{
    uint8_t Instruction[] = { 0b00000100, 0b00010001 }; // mov r2, r1

    CPU.SetRegister(Register::R1, 0xFACE);
    CPU.ExecuteSingleInstruction(Instruction);
    EXPECT_EQ(CPU.GetRegister(lce::Assembler::Register::R2), 0xFACE);
}

TEST_F(TestCPU, TestMovRegOneByteImm)
{
    uint8_t Instruction[] = { 0b00000100, 0b10001000, 0x1F }; // mov r1, 0x1F

    CPU.ExecuteSingleInstruction(Instruction);
    EXPECT_EQ(CPU.GetRegister(lce::Assembler::Register::R1), 0x1F);
}

TEST_F(TestCPU, TestMovRegTwoByteImm)
{
    uint8_t Instruction[] = { 0b00000100, 0b11001000, 0xCE, 0xFA }; // mov r1, 0x1F

    CPU.ExecuteSingleInstruction(Instruction);
    EXPECT_EQ(CPU.GetRegister(lce::Assembler::Register::R1), 0xFACE);
}
