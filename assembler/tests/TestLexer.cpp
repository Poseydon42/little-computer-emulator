#include <gtest/gtest.h>

#include "Lexer.h"

TEST(TestLexer, IgnoreWhitespaces)
{
    lce::Assembler::Lexer Lexer("\t    \t\n\r\n  ", "test_file.lca");

    EXPECT_EQ(Lexer.Peek().Type, lce::Assembler::LexemType::EndOfFile);
}

TEST(TestLexer, IdentifierBasics)
{
    lce::Assembler::Lexer Lexer("i1 i2 _i3 _i_4_", "test_file.lca");

    EXPECT_EQ(Lexer.Peek().Type, lce::Assembler::LexemType::Identifier);
    ASSERT_TRUE(std::holds_alternative<std::string>(Lexer.Peek().ParsedValue));
    EXPECT_EQ(std::get<std::string>(Lexer.Peek().ParsedValue), "i1");
    Lexer.Next();

    EXPECT_EQ(Lexer.Peek().Type, lce::Assembler::LexemType::Identifier);
    ASSERT_TRUE(std::holds_alternative<std::string>(Lexer.Peek().ParsedValue));
    EXPECT_EQ(std::get<std::string>(Lexer.Peek().ParsedValue), "i2");
    Lexer.Next();

    EXPECT_EQ(Lexer.Peek().Type, lce::Assembler::LexemType::Identifier);
    ASSERT_TRUE(std::holds_alternative<std::string>(Lexer.Peek().ParsedValue));
    EXPECT_EQ(std::get<std::string>(Lexer.Peek().ParsedValue), "_i3");
    Lexer.Next();

    EXPECT_EQ(Lexer.Peek().Type, lce::Assembler::LexemType::Identifier);
    ASSERT_TRUE(std::holds_alternative<std::string>(Lexer.Peek().ParsedValue));
    EXPECT_EQ(std::get<std::string>(Lexer.Peek().ParsedValue), "_i_4_");
    Lexer.Next();

    EXPECT_EQ(Lexer.Peek().Type, lce::Assembler::LexemType::EndOfFile);
}

TEST(TestLexer, NumericLiterals)
{
    lce::Assembler::Lexer Lexer("123 0", "test_file.lca");

    EXPECT_EQ(Lexer.Peek().Type, lce::Assembler::LexemType::NumericLiteral);
    ASSERT_TRUE(std::holds_alternative<uint64_t>(Lexer.Peek().ParsedValue));
    EXPECT_EQ(std::get<uint64_t>(Lexer.Peek().ParsedValue), 123);
    Lexer.Next();

    EXPECT_EQ(Lexer.Peek().Type, lce::Assembler::LexemType::NumericLiteral);
    ASSERT_TRUE(std::holds_alternative<uint64_t>(Lexer.Peek().ParsedValue));
    EXPECT_EQ(std::get<uint64_t>(Lexer.Peek().ParsedValue), 0);
    Lexer.Next();

    EXPECT_EQ(Lexer.Peek().Type, lce::Assembler::LexemType::EndOfFile);
}

TEST(TestLexer, Commas)
{
    lce::Assembler::Lexer Lexer("i1, i2,,", "test_file.lca");

    EXPECT_EQ(Lexer.Next().Type, lce::Assembler::LexemType::Identifier);
    EXPECT_EQ(Lexer.Next().Type, lce::Assembler::LexemType::Comma);
    EXPECT_EQ(Lexer.Next().Type, lce::Assembler::LexemType::Identifier);
    EXPECT_EQ(Lexer.Next().Type, lce::Assembler::LexemType::Comma);
    EXPECT_EQ(Lexer.Next().Type, lce::Assembler::LexemType::Comma);
}

TEST(TestLexer, SquareBrackets)
{
    lce::Assembler::Lexer Lexer("[]", "test_file.lca");

    EXPECT_EQ(Lexer.Next().Type, lce::Assembler::LexemType::LeftSquareBracket);
    EXPECT_EQ(Lexer.Next().Type, lce::Assembler::LexemType::RightSquareBracket);
}

TEST(TestLexer, Location)
{
    lce::Assembler::Lexer Lexer("i1\n  i2", "test_file.lca");

    EXPECT_EQ(Lexer.Peek().Location.FileName, "test_file.lca");
    EXPECT_EQ(Lexer.Peek().Location.Line, 1);
    EXPECT_EQ(Lexer.Peek().Location.Column, 1);
    EXPECT_EQ(Lexer.Peek().Location.LinearOffset, 0);
    Lexer.Next();

    EXPECT_EQ(Lexer.Peek().Location.Line, 2);
    EXPECT_EQ(Lexer.Peek().Location.Column, 3);
    EXPECT_EQ(Lexer.Peek().Location.LinearOffset, 5);
}

TEST(TestLexer, Comments)
{
    lce::Assembler::Lexer Lexer("i1; hello 123 \ni2", "test_file.lca");

    EXPECT_EQ(Lexer.Next().Text, "i1");
    EXPECT_EQ(Lexer.Next().Text, "i2");
}
