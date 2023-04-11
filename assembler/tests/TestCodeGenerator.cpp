#include <gtest/gtest.h>
#include <vector>

#include "CodeGenerator.h"
#include "Instruction.h"

TEST(TestCodeGenerator, NoOperands)
{
    lce::Assembler::Instruction Instruction = {};
    Instruction.Opcode = lce::Assembler::Opcode::Nop;
    Instruction.Operands[0].Type = lce::Assembler::OperandType::None;
    Instruction.Operands[1].Type = lce::Assembler::OperandType::None;

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Instruction });
    ASSERT_EQ(GeneratedBytes.size(), 1);
    EXPECT_EQ(GeneratedBytes[0], 0b01010100);
}

TEST(TestCodeGenerator, OneRegisterOperand)
{
    lce::Assembler::Instruction Instruction = {};
    Instruction.Opcode = lce::Assembler::Opcode::Pop;
    Instruction.Operands[0].Type = lce::Assembler::OperandType::Register;
    Instruction.Operands[0].Value = lce::Assembler::Register::R1;
    Instruction.Operands[1].Type = lce::Assembler::OperandType::None;

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Instruction });
    ASSERT_EQ(GeneratedBytes.size(), 2);
    EXPECT_EQ(GeneratedBytes[0], 0b00110100);
    EXPECT_EQ(GeneratedBytes[1], 0b00001000);
}

TEST(TestCodeGenerator, OneSingleByteImmediateOperand)
{
    lce::Assembler::Instruction Instruction = {};
    Instruction.Opcode = lce::Assembler::Opcode::Push;
    Instruction.Operands[0].Type = lce::Assembler::OperandType::Immediate;
    Instruction.Operands[0].Value = 42ull;
    Instruction.Operands[1].Type = lce::Assembler::OperandType::None;

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Instruction });
    ASSERT_EQ(GeneratedBytes.size(), 2);
    EXPECT_EQ(GeneratedBytes[0], 0b00110010);
    EXPECT_EQ(GeneratedBytes[1], 42);
}

TEST(TestCodeGenerator, OneDoubleByteImmediateOperand)
{
    lce::Assembler::Instruction Instruction = {};
    Instruction.Opcode = lce::Assembler::Opcode::Push;
    Instruction.Operands[0].Type = lce::Assembler::OperandType::Immediate;
    Instruction.Operands[0].Value = 420ull;
    Instruction.Operands[1].Type = lce::Assembler::OperandType::None;

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Instruction });
    ASSERT_EQ(GeneratedBytes.size(), 3);
    EXPECT_EQ(GeneratedBytes[0], 0b00110011);
    EXPECT_EQ(GeneratedBytes[1], 420 % 256);
    EXPECT_EQ(GeneratedBytes[2], 420 >> 8);
}

TEST(TestCodeGenerator, TwoRegisterOperands)
{
    lce::Assembler::Instruction Instruction = {};
    Instruction.Opcode = lce::Assembler::Opcode::Mov;
    Instruction.Operands[0].Type = lce::Assembler::OperandType::Register;
    Instruction.Operands[0].Value = lce::Assembler::Register::R2;
    Instruction.Operands[1].Type = lce::Assembler::OperandType::Register;
    Instruction.Operands[1].Value = lce::Assembler::Register::R3;

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Instruction });
    ASSERT_EQ(GeneratedBytes.size(), 2);
    EXPECT_EQ(GeneratedBytes[0], 0b00000100);
    EXPECT_EQ(GeneratedBytes[1], 0b00010011);
}

TEST(TestCodeGenerator, FirstRegisterSecondSingleByteImmediate)
{
    lce::Assembler::Instruction Instruction = {};
    Instruction.Opcode = lce::Assembler::Opcode::Lda;
    Instruction.Operands[0].Type = lce::Assembler::OperandType::Register;
    Instruction.Operands[0].Value = lce::Assembler::Register::R2;
    Instruction.Operands[1].Type = lce::Assembler::OperandType::Immediate;
    Instruction.Operands[1].Value = 42ull;

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Instruction });
    ASSERT_EQ(GeneratedBytes.size(), 3);
    EXPECT_EQ(GeneratedBytes[0], 0b00001000);
    EXPECT_EQ(GeneratedBytes[1], 0b10010000);
    EXPECT_EQ(GeneratedBytes[2], 42);
}

TEST(TestCodeGenerator, FirstRegisterSecondDoubleByteImmediate)
{
    lce::Assembler::Instruction Instruction = {};
    Instruction.Opcode = lce::Assembler::Opcode::Lda;
    Instruction.Operands[0].Type = lce::Assembler::OperandType::Register;
    Instruction.Operands[0].Value = lce::Assembler::Register::R2;
    Instruction.Operands[1].Type = lce::Assembler::OperandType::Immediate;
    Instruction.Operands[1].Value = 420ull;

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Instruction });
    ASSERT_EQ(GeneratedBytes.size(), 4);
    EXPECT_EQ(GeneratedBytes[0], 0b00001000);
    EXPECT_EQ(GeneratedBytes[1], 0b11010000);
    EXPECT_EQ(GeneratedBytes[2], 420 % 256);
    EXPECT_EQ(GeneratedBytes[3], 420 >> 8);
}

TEST(TestCodeGenerator, FirstSingleByteImmediateSecondRegister)
{
    lce::Assembler::Instruction Instruction = {};
    Instruction.Opcode = lce::Assembler::Opcode::Sta;
    Instruction.Operands[0].Type = lce::Assembler::OperandType::Immediate;
    Instruction.Operands[0].Value = 42ull;
    Instruction.Operands[1].Type = lce::Assembler::OperandType::Register;
    Instruction.Operands[1].Value = lce::Assembler::Register::R3;

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Instruction });
    ASSERT_EQ(GeneratedBytes.size(), 3);
    EXPECT_EQ(GeneratedBytes[0], 0b00001110);
    EXPECT_EQ(GeneratedBytes[1], 0b00000011);
    EXPECT_EQ(GeneratedBytes[2], 42);
}

TEST(TestCodeGenerator, FirstDoubleByteImmediateSecondRegister)
{
    lce::Assembler::Instruction Instruction = {};
    Instruction.Opcode = lce::Assembler::Opcode::Sta;
    Instruction.Operands[0].Type = lce::Assembler::OperandType::Immediate;
    Instruction.Operands[0].Value = 420ull;
    Instruction.Operands[1].Type = lce::Assembler::OperandType::Register;
    Instruction.Operands[1].Value = lce::Assembler::Register::R3;

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Instruction });
    ASSERT_EQ(GeneratedBytes.size(), 4);
    EXPECT_EQ(GeneratedBytes[0], 0b00001111);
    EXPECT_EQ(GeneratedBytes[1], 0b00000011);
    EXPECT_EQ(GeneratedBytes[2], 420 % 256);
    EXPECT_EQ(GeneratedBytes[3], 420 >> 8);
}
