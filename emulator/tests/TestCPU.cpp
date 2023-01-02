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

TEST_F(TestCPU, TestMOVRegReg)
{
    CPU.SetRegister(Register::R0, 0xABCD);

    uint8_t Instruction[] = { 0b00001001, 0b00000000 }; // mov R1, R0
    CPU.ExecuteSingleInstruction(Instruction);

    EXPECT_EQ(CPU.GetRegister(Register::R1), 0xABCD);
}

TEST_F(TestCPU, TestMOVRegImm)
{
    uint8_t Instruction[] = { 0b00010000, 0xFE, 0xCA }; // mov R0, 0xCAFE
    CPU.ExecuteSingleInstruction(Instruction);

    EXPECT_EQ(CPU.GetRegister(Register::R0), 0xCAFE);
}

TEST_F(TestCPU, TestMOVRegImmAddr)
{
    Memory->Write(0x0077, 0xCE);
    Memory->Write(0x0078, 0xBB);

    uint8_t Instruction[] = { 0b00011001, 0x77, 0x80 }; // mov R1, [0x8077]
    CPU.ExecuteSingleInstruction(Instruction);

    EXPECT_EQ(CPU.GetRegister(Register::R1), 0xBBCE);
}

TEST_F(TestCPU, TestMOVRegRegAddr)
{
    Memory->Write(0x0077, 0xCE);
    Memory->Write(0x0078, 0xA0);
    CPU.SetRegister(Register::R0, 0x8077);

    uint8_t Instruction[] = { 0b00100001, 0b00000000 };
    CPU.ExecuteSingleInstruction(Instruction); // mov R1, [R0]

    EXPECT_EQ(CPU.GetRegister(Register::R1), 0xA0CE);
}

TEST_F(TestCPU, TestMOVImmAddrReg)
{
    CPU.SetRegister(Register::R0, 0xB0C0);

    uint8_t Instruction[] = { 0b00101000, 0x77, 0x80 }; // mov [0x8077], R0
    CPU.ExecuteSingleInstruction(Instruction);

    EXPECT_EQ(Memory->Read(0x0077), 0xC0);
    EXPECT_EQ(Memory->Read(0x0078), 0xB0);
}

TEST_F(TestCPU, TestMOVRegAddrReg)
{
    CPU.SetRegister(Register::R0, 0xCAFE);
    CPU.SetRegister(Register::R1, 0x8077);

    uint8_t Instruction[] = { 0b00110001, 0b00000000 }; // mov [R1], R0
    CPU.ExecuteSingleInstruction(Instruction);

    EXPECT_EQ(Memory->Read(0x0077), 0xFE);
    EXPECT_EQ(Memory->Read(0x0078), 0xCA);
}
