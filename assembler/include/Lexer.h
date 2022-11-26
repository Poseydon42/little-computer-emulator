#pragma once

#include <string>
#include <string_view>

#include "Lexem.h"

namespace lce::Assembler
{
    class Lexer
    {
    public:
        Lexer(std::string_view Source, std::string FileName);

        /*
         * Returns the next lexem in the source stream
         */
        const Lexem& Peek() const;

        /*
         * Returns the next lexem in the source stream and shifts to the next lexem
         */
        Lexem Next();

    private:
        std::string_view m_Source;

        std::string m_FileName;

        size_t m_CurrentOffset = 0;
        SourceLocation m_CurrentLocation;

        Lexem m_CurrentLexem;

        /*
         * Returns the first character in the source stream if there are any charactes left
         * Otherwise returns null character
         */
        char PeekChar() const;

        /*
         * Moves to the next character in the input stream and updates the necessary variables
         * Returns the character that it moved to if there are any characters left in the input stream
         * Otherwise returns 0
         */
        char Advance();
    };
}
