#include <gtest/gtest.h>

#include <vector>

#include "Lexer.h"
#include "Parser.h"

TEST(TestParser, ParseSimpleMov)
{
    lce::Assembler::Lexer Lexer("mov r0, 1337 ; Test comment", "test_file.lca");
    std::vector<lce::Assembler::Instruction> Instructions;

    ASSERT_TRUE(lce::Assembler::Parse(Lexer, Instructions));
    EXPECT_EQ(Instructions.size(), 1);

    EXPECT_EQ(Instructions[0].Opcode, lce::Assembler::Opcode::Mov);

    EXPECT_EQ(Instructions[0].Operands[0].Type, lce::Assembler::OperandType::Register);
    EXPECT_EQ(std::get<lce::Assembler::Register>(Instructions[0].Operands[0].Value), lce::Assembler::Register::R0);

    EXPECT_EQ(Instructions[0].Operands[1].Type, lce::Assembler::OperandType::Immediate);
    EXPECT_EQ(std::get<uint64_t>(Instructions[0].Operands[1].Value), 1337);
}

TEST(TestParser, ParseMultipleMov)
{
    lce::Assembler::Lexer Lexer("mov r0, 0 ; instruction 1 \n mov r0, 1 ; instruction 2 \n mov r0, 2 ; instruction2 ", "test_file.lca");
    std::vector<lce::Assembler::Instruction> Instructions;

    ASSERT_TRUE(lce::Assembler::Parse(Lexer, Instructions));
    ASSERT_EQ(Instructions.size(), 3);

    for (int Index = 0; Index < 3; Index++)
    {
        EXPECT_EQ(Instructions[Index].Opcode, lce::Assembler::Opcode::Mov);

        EXPECT_EQ(Instructions[Index].Operands[0].Type, lce::Assembler::OperandType::Register);
        EXPECT_EQ(std::get<lce::Assembler::Register>(Instructions[0].Operands[0].Value), lce::Assembler::Register::R0);

        EXPECT_EQ(Instructions[Index].Operands[1].Type, lce::Assembler::OperandType::Immediate);
        EXPECT_EQ(std::get<uint64_t>(Instructions[Index].Operands[1].Value), Index);
    }
}

TEST(TestParser, ParseWithEmptyLines)
{
    lce::Assembler::Lexer Lexer("\n\n mov r0, 42 \n\n\n", "test_file.lca");
    std::vector<lce::Assembler::Instruction> Instructions;

    EXPECT_TRUE(lce::Assembler::Parse(Lexer, Instructions));
    EXPECT_EQ(Instructions.size(), 1);
}
