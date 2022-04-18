#include <string>
#include <iostream>
#include <ctime>
#include <vector>
#include <any>
#include <typeinfo>
#include <sstream>
#include <regex>
#include <iomanip>
#include <fstream>
#include <filesystem>

#define LG_INFO(...) Logging::Log::info(__VA_ARGS__)
#define LG_WARN(...) Logging::Log::warn(__VA_ARGS__)
#define LG_FATAL(...) Logging::Log::fatal(__VA_ARGS__)

namespace Logging
{

    class Log
    {
        class DecimalFormat
        {
        public:
            DecimalFormat() : format(false) {}

            DecimalFormat(std::string &formatting);

            int getPrecision() const;

            bool getFormat() const;

        private:
            int decimalPrecision;
            std::string formatter;
            bool format;
        };

        class Alignment
        {
        public:
            enum Aligned
            {
                NONE,
                LEFT,
                RIGHT,
                CENTER
            };

            Alignment() : aligned(Aligned::NONE), inUse(false) {}
            Alignment(const std::string &format);

            Aligned getAlignment() const;
            int getArgLength() const;
            int getFormatLength() const;
            bool getInUse() const;

            void setLength(const size_t length);

        private:
            Aligned aligned;
            std::string formatter;
            size_t argLength;
            int formatting;
            bool inUse;
        };

        class Truncation
        {
            enum Truncate
            {
                NONE,
                LEFT,
                RIGHT,
                CENTER
            };

        public:
            Truncation() : truncate(Truncate::NONE), inUse(false) {}
            Truncation(const std::string &format);

            Truncate getTruncation() const;

            int getArgLength() const;
            int getFormatLength() const;
            bool getInUse() const;

            void setArgument(const std::string &argument);

        private:
            std::string formatter;
            std::string arg;
            int formatting;
            size_t argLength;
            Truncate truncate;
            bool inUse;
        };

    public:
        template <class... Args>
        static void info(const std::string &logMessage, const Args &...args)
        {
            std::string newLogMessage = logMessage;

            if (newLogMessage[0] == '\n')
            {
                sendOutput("\n");
                newLogMessage.erase(0, 1);
            }

            std::cout << "\033[38;2;255;255;255m";

            sendOutput("[");

            printer(newLogMessage, args...);
        }

        template <class... Args>
        static void warn(const std::string &logMessage, const Args &...args)
        {
            std::string newLogMessage = logMessage;

            if (newLogMessage[0] == '\n')
            {
                sendOutput("\n");
                newLogMessage.erase(0, 1);
            }

            std::cout << "\033[38;2;232;129;26m";

            sendOutput("[");

            printer(newLogMessage, args...);
        }

        template <class... Args>
        static void fatal(const std::string &logMessage, const Args &...args)
        {
            std::string newLogMessage = logMessage;

            if (newLogMessage[0] == '\n')
            {
                sendOutput("\n");
                newLogMessage.erase(0, 1);
            }

            std::cout << "\033[38;2;217;28;28m";

            sendOutput("[");

            printer(newLogMessage, args...);

            exit(1);
        }

        template <typename T>
        static void sendOutput(const T &output, const DecimalFormat *decimalFormat = nullptr, const Alignment *alignment = nullptr, const Truncation *truncation = nullptr)
        {
            if (logLocation != "")
            {
                outFile.open(logLocation, std::ios::app);

                outStream(outFile, output, decimalFormat, alignment, truncation);

                outFile.close();
            }
            else
                outStream(std::cout, output, decimalFormat, alignment, truncation);
        }

        template <typename T>
        static void outStream(std::ostream &stream, const T &output, const DecimalFormat *decimalFormat = nullptr, const Alignment *alignment = nullptr, const Truncation *truncation = nullptr)
        {
            if (decimalFormat != nullptr && decimalFormat->getFormat())
                stream << std::fixed << std::setprecision(decimalFormat->getPrecision()) << output;
            else if (alignment != nullptr && alignment->getInUse())
            {
                switch (alignment->getAlignment())
                {
                case Alignment::Aligned::NONE:
                    stream << output;
                    break;
                case Alignment::Aligned::LEFT:
                    if (alignment->getArgLength() >= alignment->getFormatLength())
                        stream << output;
                    else
                        stream << std::left << std::setw(alignment->getFormatLength()) << output;
                    break;
                case Alignment::Aligned::RIGHT:
                    if (alignment->getArgLength() >= alignment->getFormatLength())
                        stream << output;
                    else
                        stream << std::right << std::setw(alignment->getFormatLength()) << output;
                    break;
                case Alignment::Aligned::CENTER:
                    if (alignment->getArgLength() >= alignment->getFormatLength())
                        stream << output;
                    else
                    {
                        const bool isOdd = alignment->getFormatLength() & 1;
                        const int mid = (alignment->getFormatLength() - alignment->getArgLength()) / 2;

                        stream << std::left << std::setw(mid + isOdd) << "";
                        stream << output;
                        stream << std::right << std::setw(mid) << "";
                    }
                    break;
                default:
                    break;
                }
            }
            else if (truncation != nullptr && truncation->getInUse())
            {
                int argLength = truncation->getArgLength();
                int formatLength = truncation->getFormatLength();

                switch (truncation->getTruncation())
                {
                case Truncation::Truncate::NONE:
                    stream << output;
                    break;
                case Truncation::Truncate::LEFT:
                    if (argLength >= formatLength)
                        stream << output;

                    break;
                case Truncation::Truncate::RIGHT:
                    if (truncation->getArgLength() >= truncation->getFormatLength())
                        stream << output;
                    else
                        stream << std::right << std::setw(truncation->getFormatLength()) << output;
                    break;
                case Truncation::Truncate::CENTER:
                    if (truncation->getArgLength() >= truncation->getFormatLength())
                        stream << output;
                    else
                    {
                        const bool isOdd = truncation->getFormatLength() & 1;
                        const int mid = (truncation->getFormatLength() - truncation->getArgLength()) / 2;

                        stream << std::left << std::setw(mid + isOdd) << "";
                        stream << output;
                        stream << std::right << std::setw(mid) << "";
                    }
                    break;
                default:
                    break;
                }
            }
            else
                stream << output;
        }

        template <class... Args>
        static void printer(const std::string &logMessage, const Args &...args)
        {
            std::vector<std::any> anyArgs = {args...};

            unsigned long argsLength = anyArgs.size();

            time_t ttime = time(nullptr);

            tm *local_time = localtime(&ttime); // TODO make time follow #timeFormatting

            std::string timeString = std::to_string(local_time->tm_hour) + ":" + std::to_string(local_time->tm_min) + ":" + std::to_string(local_time->tm_sec) + "]: " + header + ": ";

            sendOutput(timeString);

            int i = 0;

            std::string splitArgs[2];

            while (i < static_cast<int>(logMessage.length()))
            {
                if (logMessage[static_cast<unsigned long>(i)] == '{')
                {
                    argSplitter(logMessage, splitArgs, i);

                    std::regex pos("^\\d{1}$");

                    if (std::regex_match(splitArgs[0], pos))
                    {
                        unsigned long position = std::stoul(splitArgs[0]);

                        if (position > argsLength)
                            LG_FATAL("\n{0} is is greater than the provided amount of arguments in:\n\t{1}", splitArgs[0], logMessage);

                        printAtIndex(anyArgs, position, splitArgs[1], logMessage);
                    }
                    else
                        LG_FATAL("\n{0} is an invalid positional argument in:\n\t{1}", splitArgs[0], logMessage);
                }
                else
                    sendOutput(logMessage[static_cast<unsigned long>(i++)]);
            }
            sendOutput(".");

            std::cout << "\033[0m";

            sendOutput("\n");
        }

        static void printAtIndex(const std::vector<std::any> &args, const unsigned long index, std::string &formatting, const std::string &logMessage)
        {
            Alignment alignment(formatting);

            DecimalFormat decimalFormat(formatting);

            Truncation truncation(formatting);

            if (args[index].type() == typeid(short))
            {
                short value = std::any_cast<short>(args[index]);

                std::string output = std::to_string(value);

                alignment.setLength(output.length());

                truncation.setArgument(output);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(unsigned short))
            {
                unsigned short value = std::any_cast<unsigned short>(args[index]);

                std::string output = std::to_string(static_cast<int>(value));

                alignment.setLength(output.length());

                truncation.setArgument(output);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(int))
            {
                int value = std::any_cast<int>(args[index]);

                std::string output = std::to_string(value);

                alignment.setLength(output.length());

                truncation.setArgument(output);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(unsigned int))
            {
                unsigned int value = std::any_cast<unsigned int>(args[index]);

                std::string output = std::to_string(value);

                alignment.setLength(output.length());

                truncation.setArgument(output);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(long))
            {
                long value = std::any_cast<long>(args[index]);

                std::string output = std::to_string(value);

                alignment.setLength(output.length());

                truncation.setArgument(output);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(unsigned long))
            {
                unsigned long value = std::any_cast<unsigned long>(args[index]);

                std::string output = std::to_string(value);

                alignment.setLength(output.length());

                truncation.setArgument(output);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(float))
            {
                float value = std::any_cast<float>(args[index]);

                std::string output = std::to_string(value);

                alignment.setLength(output.length());

                truncation.setArgument(output);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(double))
            {
                double value = std::any_cast<double>(args[index]);

                std::string output = std::to_string(value);

                alignment.setLength(output.length());

                truncation.setArgument(output);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(std::string))
            {
                std::string value = std::any_cast<std::string>(args[index]);

                alignment.setLength(value.length());

                truncation.setArgument(value);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(bool))
            {
                std::string value = std::any_cast<bool>(args[index]) ? "true" : "false";

                alignment.setLength(value.length());

                truncation.setArgument(value);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else if (std::strcmp(args[index].type().name(), "PKc") == 0)
            {
                const char *value = std::any_cast<const char *>(args[index]);

                std::string inString(value);

                alignment.setLength(inString.length());

                truncation.setArgument(inString);

                handlePrintAtIndex(value, decimalFormat, alignment, truncation);
            }
            else
                LG_FATAL("\nArgument {0} is not an allowed type to be printed in:\n\t{1}", index, logMessage);
        }

        template <typename T>
        static void handlePrintAtIndex(const T &val, const DecimalFormat &decimalFormat, const Alignment &alignment, const Truncation &truncation)
        {
            sendOutput(val, &decimalFormat, &alignment, &truncation);
        }

        static void argSplitter(const std::string &argument, std::string *argArray, int &index)
        {
            // Get the argument within the braces
            std::string enclosed = "";

            while (argument[static_cast<unsigned long>(++index)] != '}')
                enclosed += argument[static_cast<unsigned long>(index)];

            index++;

            // Split the argument on :

            const char DELIMITER = ':';

            // If : not found
            if (enclosed.find(DELIMITER) == std::string::npos)
            {
                argArray[0] = enclosed;
                argArray[1] = "";
            }
            else
            {
                size_t pos = 0;
                std::string token;

                std::stringstream ss(enclosed);

                while (std::getline(ss, token, DELIMITER))
                    argArray[pos++] = token;
            }
        }

        void setTimeFormatting(const std::string &format);

        void setHeader(const std::string &header);

        void setLogInfo(const std::string &folder, const std::string &file);

    private:
        static std::string timeFormatting;
        static std::string header;
        static std::filesystem::path logLocation;
        static std::ofstream outFile;
    };
}
