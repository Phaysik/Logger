#include "log.h"

std::string Logging::Log::header;
std::string Logging::Log::timeFormatting;
std::filesystem::path Logging::Log::logLocation;
std::ofstream Logging::Log::outFile;

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

void Logging::Log::setLogInfo(const std::string &folder, const std::string &file)
{
    logLocation = {folder};

    logLocation /= file;

    if (!std::filesystem::exists(folder))
    {
        std::filesystem::create_directories(logLocation.parent_path());

        LG_INFO("{0} does not exist. Creating that directory now.", folder);
    }

    outFile.open(logLocation);

    outFile.close();
}
