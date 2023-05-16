#include <gtest/gtest.h>

#include "Instruction.h"
#include "TypeChecker.h"

using namespace lce::Assembler;

TEST(TestTypeChecker, Succeeds)
{
    Instruction Instructions[2] = {
        { Opcode::Mov, { { OperandType::Register, Register::R0 }, { OperandType::Immediate, 34u } } },
        { Opcode::Mov, { { OperandType::Register, Register::R3 }, { OperandType::Register, Register::R0 } } }
    };

    EXPECT_TRUE(CheckInstructionSequence(Instructions));
}

TEST(TestTypeChecker, Fails)
{
    Instruction Instructions[2] = {
        { Opcode::Mov, { { OperandType::Immediate, 34u }, { OperandType::Register, Register::R0 } } },
        { Opcode::Mov, { { OperandType::Register, Register::R3 }, { OperandType::Register, Register::R0 } } }
    };

    EXPECT_FALSE(CheckInstructionSequence(Instructions));
}
