#include "log.h"

std::string Logging::Log::header;
std::string Logging::Log::timeFormatting;

void Logging::Log::setTimeFormatting(const std::string &format)
{
    std::regex timeFormattingRegex("%([HMS]):%((?!\\1)[HMS]):%(?!\\1)(?!\\2)[HMS]");

    if (std::regex_match(format, timeFormattingRegex))
    {
        timeFormatting = format;
    }
    else
    {
        LG_WARN("{0} is an invalid time formatting. The formatting will default to %H:%M:%S", format);

        timeFormatting = "%H:%M:%S";
    }
}

void Logging::Log::setHeader(const std::string &logHeader)
{
    header = logHeader;
}
