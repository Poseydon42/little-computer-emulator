#pragma once

#include <fmt/format.h>
#include <iostream>
#include <type_traits>
#include <utility>

#include "SourceLocation.h"

namespace lce::Common
{
    enum class ErrorSeverity
    {
        Info,
        Warning,
        Error,
        Fatal
    };

    /*
     * Adds infomation about error severity and location of the error in the source file to the formatted message
     */
    std::string ApplyGlobalFormattingToMessage(ErrorSeverity Severity, const SourceLocation& Location, const char* FormattedMessage);

    /*
     * Prints the formatted message to the correct destination (e.g. stdout/stderr, file log etc.)
     */
    void PrintFormattedError(ErrorSeverity Severity, const char* Message);

    template <typename... ArgTypes>
    void ReportError(ErrorSeverity Severity, const SourceLocation& Location, const char* Message, ArgTypes&&... Args)
    {
        // FIXME: let user change this
        constexpr ErrorSeverity LoggingLevel = ErrorSeverity::Warning;

        if (Severity < LoggingLevel)
            return;

        auto FormattedMessageWithoutSourceLocation = fmt::vformat(Message, fmt::make_format_args(std::forward<ArgTypes>(Args)...));
        auto FormattedMessageWithSourceLocation = ApplyGlobalFormattingToMessage(Severity, Location, FormattedMessageWithoutSourceLocation.c_str());

        using ErrorSeverityUnderlyingType = std::underlying_type<ErrorSeverity>::type;
        if (static_cast<ErrorSeverityUnderlyingType>(Severity) >= static_cast<ErrorSeverityUnderlyingType>(LoggingLevel))
        {
            std::cerr << FormattedMessageWithSourceLocation << std::endl;
        }
        else
        {
            std::cout << FormattedMessageWithSourceLocation << std::endl;
        }
    }
} // namespace lce::Common
