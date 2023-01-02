#include <gtest/gtest.h>

#include "CPU.h"

using namespace lce::Assembler;
using namespace lce::Emulator;

TEST(TestCPU, TestReset)
{
    CPU CPU;
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
