#include "Parser.h"

#include <algorithm>
#include <cassert>
#include <optional>

#include "ErrorReporting.h"

namespace lce::Assembler
{
    std::optional<Register> GetRegisterFromText(const Lexem& Lexem)
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

    // NOTE: all *Parser functions assume that the current lexem in the lexem is the lexem that contains opcode of the instruction
    std::optional<Instruction> MovParser(Lexer& Lexer)
    {
        Instruction Result = {};
        Result.Opcode = Opcode::Mov;

        auto InstructionLine = Lexer.Peek().Location.Line;

        // Skipping the opcode instruction
        Lexer.Next();

        // First operand
        if (Lexer.Peek().Location.Line != InstructionLine)
        {
            Common::ReportError(Common::ErrorSeverity::Error, Lexer.Peek().Location, "could not parse the first argument of 'mov' instruction: new line detected");
            return {};
        }

        // FIXME: mov [Imm], xxx
        // NOTE: currently we are assuming that the first argument is a register
        if (Lexer.Peek().Type != LexemType::Identifier || !GetRegisterFromText(Lexer.Peek()).has_value())
        {
            Common::ReportError(Common::ErrorSeverity::Error, Lexer.Peek().Location, "the first argument of 'mov' instruction must be a register");
            return {};
        }
        Result.Operands[0] = { OperandType::Register, GetRegisterFromText(Lexer.Next()).value() };

        // Second operand
        if (Lexer.Peek().Type != LexemType::Comma)
        {
            Common::ReportError(Common::ErrorSeverity::Error, Lexer.Peek().Location, "expected comma after the first argument, got {}", LexemTypeAsString.at(Lexer.Peek().Type));
            return {};
        }
        if (Lexer.Peek().Location.Line != InstructionLine)
        {
            Common::ReportError(Common::ErrorSeverity::Error, Lexer.Peek().Location, "could not parse second argument of 'mov' instruction: new line detected");
            return {};
        }
        Lexer.Next();

        // FIXME: at the moment we can only parse immediate value as the second argument
        if (Lexer.Peek().Location.Line != InstructionLine)
        {
            Common::ReportError(Common::ErrorSeverity::Error, Lexer.Peek().Location, "could not parse second argument of 'mov' instruction: new line detected");
            return {};
        }
        if (Lexer.Peek().Type != LexemType::NumericLiteral)
        {
            Common::ReportError(Common::ErrorSeverity::Error, Lexer.Peek().Location, "expected numerical literal as the second argument of 'mov', got {}", LexemTypeAsString.at(Lexer.Peek().Type));
            return {};
        }
        Result.Operands[1] = { OperandType::Immediate, std::get<uint64_t>(Lexer.Next().ParsedValue) };

        return Result;
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

    using InstructionParserFunctionType = std::optional<Instruction> (*)(Lexer& Lexer);

    static std::optional<InstructionParserFunctionType> GetParserForInstructionOpcode(Opcode Opcode)
    {
        const static std::unordered_map<enum Opcode, InstructionParserFunctionType> ParserDictionary = {
            { Opcode::Mov, MovParser }
        };
        if (!ParserDictionary.contains(Opcode))
            return {};
        return ParserDictionary.at(Opcode);
    }

    static std::optional<Instruction> ParseNextInstruction(Lexer& Lexer)
    {
        auto InstructionLine = Lexer.Peek().Location.Line;

        auto OpcodeLexem = Lexer.Peek();
        auto Opcode = DetectOpcode(OpcodeLexem);
        if (!Opcode.has_value())
        {
            Common::ReportError(Common::ErrorSeverity::Error, OpcodeLexem.Location, "unknown or unimplemented instruction '{}'", OpcodeLexem.Text);
            return {};
        }
        auto OpcodeParser = GetParserForInstructionOpcode(Opcode.value());
        if (!OpcodeParser.has_value())
        {
            Common::ReportError(Common::ErrorSeverity::Fatal, OpcodeLexem.Location, "cannot find parser for opcode");
            return {};
        }

        auto ParsedInstruction = OpcodeParser.value()(Lexer);
        if (Lexer.Peek().Location.Line == InstructionLine && Lexer.Peek().Type != lce::Assembler::LexemType::EndOfFile)
        {
            // NOTE: do not emit warning about ignored text if the instruction parsing failed because it might have been
            //       caused by the fact that instruction parser returned in the middle of the line
            if (ParsedInstruction.has_value())
            {
                Common::ReportError(Common::ErrorSeverity::Warning, Lexer.Peek().Location, "any text after the instruction in the current line will be ignored");
            }
            while (Lexer.Next().Location.Line == InstructionLine && Lexer.Peek().Type != lce::Assembler::LexemType::EndOfFile) {}
        }

        return ParsedInstruction;
    }

    bool Parse(Lexer& Lexer, std::vector<Instruction>& Destination)
    {
        bool Success = true;

        while (Lexer.Peek().Type != LexemType::EndOfFile)
        {
            auto Instruction = ParseNextInstruction(Lexer);
            if (!Instruction.has_value())
            {
                Success = false;
            }
            else
            {
                Destination.push_back(Instruction.value());
            }
        }
        return Success;
    }
} // namespace lce::Assembler
