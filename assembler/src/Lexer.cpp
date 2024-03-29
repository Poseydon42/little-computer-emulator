#include "Lexer.h"

#include <algorithm>
#include <cstdlib>
#include <optional>

namespace lce::Assembler
{
    static bool IsAlphabetical(char Value)
    {
        return (Value >= 'a' && Value <= 'z') || (Value >= 'A' && Value <= 'Z');
    }

    static bool IsDigit(char Value)
    {
        return (Value >= '0' && Value <= '9');
    }

    static bool CanStartIdentifier(char Value)
    {
        return (IsAlphabetical(Value) || Value == '_');
    }

    static bool CanBeInsideIdentifier(char Value)
    {
        return (CanStartIdentifier(Value) || IsDigit(Value));
    }

    static bool IsWhitespace(char Value)
    {
        // Technically \0 is not a whitespace character but I've decided to include it here
        // because it makes the lexing code shorter and easier to understand
        // Also note that line break (\n) is not included here because it is considered to be a separate lexem
        constexpr char WhitespaceCharacters[] = { '\0', ' ', '\t', '\r' };
        return std::ranges::find(WhitespaceCharacters, Value) != std::end(WhitespaceCharacters);
    }

    static std::optional<LexemType> TryParseSingleCharLexem(char Value)
    {
        static const std::unordered_map<char, LexemType> SingleCharLexems = {
            { ',', LexemType::Comma },
            { '[', LexemType::LeftSquareBracket },
            { ']', LexemType::RightSquareBracket }
        };
        if (!SingleCharLexems.contains(Value))
            return std::nullopt;
        return SingleCharLexems.at(Value);
    }

    Lexer::Lexer(std::string_view Source, std::string FileName)
        : m_Source(Source), m_FileName(std::move(FileName))
    {
        m_CurrentLocation.FileName = m_FileName;
        Pop(); // Parse the first lexem
    }

    const Lexem& Lexer::Peek() const
    {
        return m_CurrentLexem;
    }

    Lexem Lexer::Pop()
    {
        Lexem Result = m_CurrentLexem;

        Lexem NewLexem = {};

        char Current = PeekChar();
        bool InsideComment = false;
        while (true)
        {
            if (Current == 0)
            {
                NewLexem.Type = LexemType::EndOfFile;
                NewLexem.Location = m_CurrentLocation;
                NewLexem.Text = "";

                break;
            }

            if (Current == '\n')
            {
                InsideComment = false;
                NewLexem = { LexemType::LineBreak, m_CurrentLocation, "\n", {} };
                Current = Advance();

                break;
            }

            if (InsideComment)
            {
                Current = Advance();
                continue;
            }

            if (IsWhitespace(Current))
            {
                Current = Advance();
            }

            if (Current == ';')
            {
                InsideComment = true;
            }

            if (CanStartIdentifier(Current))
            {
                auto StartOffset = m_CurrentOffset;
                auto StartLocation = m_CurrentLocation;

                while (CanBeInsideIdentifier(Current))
                {
                    Current = Advance();
                }

                NewLexem.Type = LexemType::Identifier;
                NewLexem.Location = StartLocation;
                NewLexem.Text = std::string_view(m_Source.data() + StartOffset, m_CurrentOffset - StartOffset);
                NewLexem.ParsedValue = std::string(NewLexem.Text);

                break;
            }

            if (IsDigit(Current))
            {
                auto StartOffset = m_CurrentOffset;
                auto StartLocation = m_CurrentLocation;

                while (IsDigit(Current))
                {
                    Current = Advance();
                }

                NewLexem.Type = LexemType::NumericLiteral;
                NewLexem.Location = StartLocation;
                NewLexem.Text = std::string_view(m_Source.data() + StartOffset, m_CurrentOffset - StartOffset);
                NewLexem.ParsedValue = std::strtoull(NewLexem.Text.data(), nullptr, 0);

                break;
            }

            if (auto MaybeSingleCharLexem = TryParseSingleCharLexem(Current); MaybeSingleCharLexem.has_value())
            {
                NewLexem.Type = MaybeSingleCharLexem.value();
                NewLexem.Location = m_CurrentLocation;
                NewLexem.Text = std::string_view(m_Source.data() + m_CurrentOffset, 1);

                Current = Advance();

                break;
            }
        }

        m_CurrentLexem = NewLexem;

        return Result;
    }

    char Lexer::PeekChar() const
    {
        if (m_CurrentOffset < m_Source.length())
        {
            return m_Source[m_CurrentOffset];
        }

        return 0;
    }

    char Lexer::Advance()
    {
        if (m_CurrentOffset + 1 < m_Source.length())
        {
            char CurrentChar = m_Source[m_CurrentOffset++];

            if (CurrentChar == '\n')
            {
                m_CurrentLocation.Line++;
                m_CurrentLocation.Column = 1;
            }
            else
            {
                m_CurrentLocation.Column++;
            }
            
            m_CurrentLocation.LinearOffset = m_CurrentOffset;
            return PeekChar();
        }

        // Go one character further than the end of the input stream so that we don't
        // read the last character forever but rather read null character after this point.<
        if (m_CurrentOffset < m_Source.length())
        {
            m_CurrentOffset++;
            m_CurrentLocation.LinearOffset = m_CurrentOffset;
        }

        return 0;
    }
}
