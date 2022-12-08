#include <gtest/gtest.h>
#include <vector>

#include "CodeGenerator.h"
#include "Instruction.h"

TEST(TestCodeGenerator, GenerateSimpleMov)
{
    lce::Assembler::Instruction Mov = {};
    Mov.Opcode = lce::Assembler::Opcode::Mov;
    Mov.Operands[0] = { lce::Assembler::OperandType::Register, lce::Assembler::Register::R0 };
    Mov.Operands[1] = { lce::Assembler::OperandType::Immediate, 1337u };

    auto GeneratedBytes = lce::Assembler::GenerateMachineCode({ Mov });
    ASSERT_EQ(GeneratedBytes.size(), 3);
    EXPECT_EQ(GeneratedBytes[0], 0b00010000);
    EXPECT_EQ(GeneratedBytes[1], 0b00111001);
    EXPECT_EQ(GeneratedBytes[2], 0b00000101);
}
