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
    }
    else if (std::regex_match(formatter, rightAligned))
    {
        formatter.erase(0, 1);

        aligned = Aligned::RIGHT;

        formatting = std::stoi(formatter);
    }
    else if (std::regex_match(formatter, centerAligned))
    {
        formatter.erase(0, 1);

        aligned = Aligned::CENTER;

        formatting = std::stoi(formatter);
    }
    else
        aligned = Aligned::NONE;
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

void Logging::Log::Alignment::setLength(const size_t length)
{
    argLength = length;
}