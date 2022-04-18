#include "log.h"

std::string Logging::Log::header;
std::string Logging::Log::timeFormatting;
std::filesystem::path Logging::Log::logLocation;
std::ofstream Logging::Log::outFile;

void Logging::Log::setTimeFormatting(const std::string &format)
{
    std::regex timeFormattingRegex("^%([HMS]):%((?!\\1)[HMS]):%(?!\\1)(?!\\2)[HMS]$");

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

Logging::Log::DecimalFormat::DecimalFormat(std::string &formatting) : formatter(formatting)
{
    std::regex decimalRegex("^0?.\\d*f$");

    if (std::regex_match(formatter, decimalRegex))
    {
        formatter.pop_back();

        format = true;

        decimalPrecision = static_cast<int>(std::stof(formatting) * 10);
    }
    else
        format = false;
}

int Logging::Log::DecimalFormat::getPrecision() const
{
    return decimalPrecision;
}

bool Logging::Log::DecimalFormat::getFormat() const
{
    return format;
}

Logging::Log::Alignment::Alignment(const std::string &format) : formatter(format)
{
    std::regex leftAligned("^<\\d+$"), rightAligned("^>\\d+$"), centerAligned("^=\\d+$");

    if (std::regex_match(formatter, leftAligned))
    {
        formatter.erase(0, 1);

        aligned = Aligned::LEFT;

        formatting = std::stoi(formatter);

        inUse = true;
    }
    else if (std::regex_match(formatter, rightAligned))
    {
        formatter.erase(0, 1);

        aligned = Aligned::RIGHT;

        formatting = std::stoi(formatter);

        inUse = true;
    }
    else if (std::regex_match(formatter, centerAligned))
    {
        formatter.erase(0, 1);

        aligned = Aligned::CENTER;

        formatting = std::stoi(formatter);

        inUse = true;
    }
    else
    {
        aligned = Aligned::NONE;
        inUse = false;
    }
}

Logging::Log::Alignment::Aligned Logging::Log::Alignment::getAlignment() const
{
    return aligned;
}

int Logging::Log::Alignment::getArgLength() const
{
    return static_cast<int>(argLength);
}

int Logging::Log::Alignment::getFormatLength() const
{
    return formatting;
}

bool Logging::Log::Alignment::getInUse() const
{
    return inUse;
}

void Logging::Log::Alignment::setLength(const size_t length)
{
    argLength = length;
}

Logging::Log::Truncation::Truncation(const std::string &format) : formatter(format)
{
    std::regex truncateRight("^-\\d+!$"), truncateLeft("^\\d+!$"), truncateCenter("^=\\d+!$");

    if (std::regex_match(formatter, truncateRight))
    {
        formatter.erase(0, 1); // The -
        formatter.pop_back();  // The !

        truncate = Truncate::RIGHT;

        formatting = std::stoi(formatter);

        inUse = true;
    }
    else if (std::regex_match(formatter, truncateLeft))
    {
        formatter.pop_back(); // The !

        truncate = Truncate::LEFT;

        formatting = std::stoi(formatter);

        inUse = true;
    }
    else if (std::regex_match(formatter, truncateCenter))
    {
        formatter.erase(0, 1); // The =
        formatter.pop_back();  // The !

        truncate = Truncate::CENTER;

        formatting = std::stoi(formatter);

        inUse = true;
    }
    else
    {
        truncate = Truncate::NONE;
        inUse = true;
    }
}

Logging::Log::Truncation::Truncate Logging::Log::Truncation::getTruncation() const
{
    return truncate;
}

int Logging::Log::Truncation::getArgLength() const
{
    return static_cast<int>(argLength);
}

int Logging::Log::Truncation::getFormatLength() const
{
    return formatting;
}

bool Logging::Log::Truncation::getInUse() const
{
    return inUse;
}

std::string Logging::Log::Truncation::getArgument() const
{
    return arg;
}

void Logging::Log::Truncation::setArgument(const std::string &argument)
{
    arg = argument;

    argLength = arg.length();
}