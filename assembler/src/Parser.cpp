#include "Parser.h"

#include <array>
#include <algorithm>
#include <cassert>
#include <optional>
#include <span>

#include "ErrorReporting.h"
#include "Instruction.h"
#include "Lexem.h"

namespace lce::Assembler
{
    static std::optional<Register> GetRegisterFromText(const Lexem& Lexem)
    {
        const static std::unordered_map<std::string, Register> RegisterNameDictionary = {
            { "r0", Register::R0 },
            { "r1", Register::R1 },
            { "r2", Register::R2 },
            { "r3", Register::R3 },
            { "rsp", Register::RSP },
            { "rfl", Register::RFL }
        };
        
        if (Lexem.Type != LexemType::Identifier)
        {
            return {};
        }
        auto LexemText = std::get<std::string>(Lexem.ParsedValue);
        std::transform(LexemText.begin(), LexemText.end(), LexemText.begin(), [](const char Char) { return std::tolower(Char); });

        if (!RegisterNameDictionary.contains(LexemText))
        {
            return {};
        }
        return RegisterNameDictionary.at(LexemText);
    }

    static std::optional<Opcode> DetectOpcode(const Lexem& IdentifierLexem)
    {
        if (IdentifierLexem.Type != LexemType::Identifier)
            return {};

        auto LexemText = std::get<std::string>(IdentifierLexem.ParsedValue);
        std::transform(LexemText.begin(), LexemText.end(), LexemText.begin(), [](const auto& Char) { return std::tolower(Char); });

        const static std::unordered_map<std::string, Opcode> TextToOpcodeMap = {
            { "mov", Opcode::Mov },
        };

        if (!TextToOpcodeMap.contains(LexemText))
        {
            return {};
        }

        return TextToOpcodeMap.at(LexemText);
    }

    static size_t GetNumberOfOperandsForOpcode(Opcode InOpcode)
    {
        static std::unordered_map<Opcode, size_t> NumberOfOperands = {
            { Opcode::Mov, 2 }
        };

        if (!NumberOfOperands.contains(InOpcode))
        {
            return 0;
        }
        return NumberOfOperands.at(InOpcode);
    }

    static std::optional<Operand> ParseOperand(std::span<Lexem> Lexems)
    {
        if (Lexems.empty())
            return {};

        // OperandType::Register
        if (Lexems[0].Type == LexemType::Identifier)
        {
            auto MaybeRegister = GetRegisterFromText(Lexems[0]);
            if (!MaybeRegister.has_value())
                return {};
            return Operand{ OperandType::Register, MaybeRegister.value() };
        }

        // OperandType::Immediate
        if (Lexems[0].Type == LexemType::NumericLiteral)
        {
            return Operand{ OperandType::Immediate, std::get<uint64_t>(Lexems[0].ParsedValue) };
        }

        // OperandType::ImmediateAddress/OperandType::RegisterAddress
        if (Lexems[0].Type == LexemType::LeftSquareBracket)
        {
            if (Lexems.size() <= 1)
            {
                Common::ReportError(Common::ErrorSeverity::Error, Lexems[0].Location, "expected address or register after the left square bracket");
                return {};
            }
            if (Lexems.size() <= 2 || Lexems[2].Type != LexemType::RightSquareBracket)
            {
                Common::ReportError(Common::ErrorSeverity::Error, Lexems[1].Location, "expected right square bracket");
                return {};
            }

            // OperandType::RegisterAddress
            if (Lexems[1].Type == LexemType::Identifier)
            {
                auto MaybeRegister = GetRegisterFromText(Lexems[1]);
                if (!MaybeRegister.has_value())
                    return {};
                return Operand{ OperandType::RegisterAddress, MaybeRegister.value() };
            }

            // OperandType::ImmediateAddress
            if (Lexems[1].Type == LexemType::NumericLiteral)
            {
                return Operand{ OperandType::Immediate, std::get<uint64_t>(Lexems[0].ParsedValue) };
            }
        }
        return {};
    }

    std::optional<Instruction> ParseInstruction(std::span<Lexem> Lexems)
    {
        Instruction Result = {};

        auto OpcodeLexem = Lexems[0];
        auto MaybeOpcode = DetectOpcode(OpcodeLexem);
        if (!MaybeOpcode.has_value())
        {
            Common::ReportError(Common::ErrorSeverity::Error, OpcodeLexem.Location, "unknown or unimplemented instruction '{}'", OpcodeLexem.Text);
            return {};
        }
        auto Opcode = MaybeOpcode.value();

        Result.Opcode = Opcode;

        size_t OnePastLastLexemOfFirstOperand = -1;
        
        auto NumberOfOperands = GetNumberOfOperandsForOpcode(Opcode);
        if (NumberOfOperands == 2)
        {
            // FIXME: this assumes that commas are not allowed inside the argument itself, and while it is true at the moment this might change in the
            //        future if we'd add support for complex mathematical expressions or macros
            auto CommaLexem = std::ranges::find_if(Lexems, [](const auto& Element) { return Element.Type == LexemType::Comma; });
            if (CommaLexem == Lexems.end())
            {
                Common::ReportError(Common::ErrorSeverity::Error, (CommaLexem - 1)->Location, "expected two arguments for opcode");
                return {};
            }

            OnePastLastLexemOfFirstOperand = std::distance(Lexems.begin(), CommaLexem);
        }
        else if (NumberOfOperands == 1)
        {
            OnePastLastLexemOfFirstOperand = Lexems.size();
        }

        if (NumberOfOperands > 0)
        {
            auto FirstOperandLexems = std::span<Lexem>(Lexems.begin() + 1, Lexems.begin() + OnePastLastLexemOfFirstOperand);
            auto MaybeFirstOperand = ParseOperand(FirstOperandLexems);
            if (!MaybeFirstOperand.has_value())
            {
                return {};
            }

            Result.Operands[0] = MaybeFirstOperand.value();
        }

        if (NumberOfOperands > 1)
        {
            auto SecondOperandLexems = std::span<Lexem>(Lexems.begin() + OnePastLastLexemOfFirstOperand + 1, Lexems.end());
            auto MaybeSecondOperand = ParseOperand(SecondOperandLexems);
            if (!MaybeSecondOperand.has_value())
            {
                return {};
            }

            Result.Operands[1] = MaybeSecondOperand.value();
        }

        return Result;
    }

    bool Parse(Lexer& Lexer, std::vector<Instruction>& Destination)
    {
        bool Success = true;

        while (Lexer.Peek().Type != LexemType::EndOfFile)
        {
            std::vector<Lexem> LexemsInCurrentLine;
            while (Lexer.Peek().Type != LexemType::LineBreak && Lexer.Peek().Type != LexemType::EndOfFile)
            {
                LexemsInCurrentLine.push_back(Lexer.Pop());
            }
            Lexer.Pop(); // Popping the line break lexem

            if (LexemsInCurrentLine.empty())
                continue;

            auto MaybeInstruction = ParseInstruction(LexemsInCurrentLine);

            if (!MaybeInstruction.has_value())
            {
                Success = false;
                continue;
            }

            auto Instruction = MaybeInstruction.value();

            Destination.push_back(Instruction);
        }
        return Success;
    }
} // namespace lce::Assembler
