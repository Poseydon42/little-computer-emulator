#include "ErrorReporting.h"

#include <iostream>
#include <stdarg.h>
#include <unordered_map>

namespace lce::Common
{
    std::string ApplyGlobalFormattingToMessage(ErrorSeverity Severity, const SourceLocation& Location, const char* FormattedMessage)
    {
        static const std::unordered_map<ErrorSeverity, std::string_view> ErrorSeverityString = {
            { ErrorSeverity::Info, "Info" },
            { ErrorSeverity::Warning, "Warning" },
            { ErrorSeverity::Error, "Error" },
            { ErrorSeverity::Fatal, "Fatal" }
        };
        auto Result = fmt::format("{} at {}({}:{}): {}", ErrorSeverityString.at(Severity), Location.FileName, Location.Line, Location.Column, FormattedMessage);
        return Result;
    }

    void PrintFormattedError(ErrorSeverity Severity, const char* Message)
    {
        constexpr ErrorSeverity MinSeverityForStderr = ErrorSeverity::Error;
        if (static_cast<std::underlying_type<ErrorSeverity>::type>(Severity) >= static_cast<std::underlying_type<ErrorSeverity>::type>(MinSeverityForStderr))
        {
            std::cerr << Message << std::endl;
        }
        else
        {
            std::cout << Message << std::endl;
        }
    }
} // namespace lce::Common
