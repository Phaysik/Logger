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
#define LG_TEST_SUCCESS(...) Logging::Log::testSuccess(__VA_ARGS__)
#define LG_TEST_FAIL(...) Logging::Log::testFailure(__VA_ARGS__)

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
        public:
            enum Truncate
            {
                NONE,
                LEFT,
                RIGHT,
                CENTER
            };

            Truncation() : truncate(Truncate::NONE), inUse(false) {}
            Truncation(const std::string &format);

            Truncate getTruncation() const;

            int getArgLength() const;
            int getFormatLength() const;
            bool getInUse() const;
            std::string getArgument() const;

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
        ~Log()
        {
            if (logLocation != "")
            {
                outFile.open(logLocation, std::ios::app);

                if (headerSet)
                    outStream(outFile, "\\end{flushleft}");

                outStream(outFile, "\\end{document}");

                outFile.close();
            }
        }
        struct RGB
        {
            short red, green, blue;

            std::string name;

            std::string toString() const;
        };

        template <class... Args>
        static void info(const std::string &logMessage, const bool ignoreFile = false, const Args &...args)
        {
            loggerAbstraction(loggerInfoColor, logMessage, ignoreFile, args...);
        }

        template <class... Args>
        static void warn(const std::string &logMessage, const bool ignoreFile = false, const Args &...args)
        {
            loggerAbstraction(loggerWarnColor, logMessage, ignoreFile, args...);
        }

        template <class... Args>
        static void fatal(const std::string &logMessage, const bool ignoreFile = false, const Args &...args)
        {
            loggerAbstraction(loggerFatalColor, logMessage, ignoreFile, args...);

            exit(1);
        }

        template <class... Args>
        static void testSuccess(const std::string &logMessage, const bool ignoreFile = false, const Args &...args)
        {
            loggerAbstraction(loggerTestSuccessColor, logMessage, ignoreFile, args...);
        }

        template <class... Args>
        static void testFailure(const std::string &logMessage, const bool ignoreFile = false, const Args &...args)
        {
            loggerAbstraction(loggerFatalColor, logMessage, ignoreFile, args...);
        }

        template <class... Args>
        static void loggerAbstraction(const RGB &coloredText, const std::string &logMessage, const bool ignoreFile, const Args &...args)
        {
            std::string newLogMessage = logMessage;

            while (newLogMessage[0] == '\n' || newLogMessage[0] == '\t')
            {
                if (newLogMessage[0] == '\n')
                    sendOutput("\n", ignoreFile);
                else
                    sendOutput("\t", ignoreFile);

                newLogMessage.erase(0, 1);
            }

            outFile.open(logLocation, std::ios::app);

            if (headerSet)
                outStream(outFile, "\\hspace{\\parindent} ");

            outStream(outFile, "\\textcolor{" + coloredText.name + "}{");

            outFile.close();

            sendOutput("[", ignoreFile);

            printer(newLogMessage, ignoreFile, args...);

            outFile.open(logLocation, std::ios::app);

            outStream(outFile, "}\n\n");

            outFile.close();
        }

        void setTimeFormatting(const std::string &format);

        void setHeader(const std::string &header);

        void setLogInfo(const std::string &folder, const std::string &file, const std::string &fileAuthor);

    private:
        static std::string timeFormatting;
        static std::string header;
        static bool headerSet;
        static std::filesystem::path logLocation;
        static std::ofstream outFile;
        static std::string author;
        static RGB loggerInfoColor;
        static RGB loggerWarnColor;
        static RGB loggerFatalColor;
        static RGB loggerTestSuccessColor;

        static void setTimeFormat(std::string &timeFormat, const size_t index, const tm *local_time)
        {
            if (timeFormatting[index] == 'H')
                timeFormat += std::to_string(local_time->tm_hour);
            else if (timeFormatting[index] == 'M')
                timeFormat += std::to_string(local_time->tm_min);
            else if (timeFormatting[index] == 'S')
                timeFormat += std::to_string(local_time->tm_sec);
        }

        static std::string getLogColor(const RGB &color)
        {
            return "\033[38;2;" + std::to_string(color.red) + ";" + std::to_string(color.green) + ";" + std::to_string(color.blue) + "m";
        }

        static void initializeFile()
        {
            outFile.open(logLocation, std::ios::app);

            outStream(outFile, "\\documentclass[12pt, a4paper]{article}\n");
            outStream(outFile, "\\usepackage{xcolor}\n");
            outStream(outFile, "\\usepackage[a3paper, total={10in, 8in}]{geometry}\n\n");

            outStream(outFile, "\\title{Logging Results}\n");
            outStream(outFile, "\\author{" + author + "}\n\n");

            outStream(outFile, "\\definecolor{loggerInfoColor}{RGB}{" + loggerInfoColor.toString() + "}\n");
            outStream(outFile, "\\definecolor{loggerWarnColor}{RGB}{" + loggerWarnColor.toString() + "}\n");
            outStream(outFile, "\\definecolor{loggerFatalColor}{RGB}{" + loggerFatalColor.toString() + "}\n");
            outStream(outFile, "\\definecolor{loggerTestSuccessColor}{RGB}{" + loggerTestSuccessColor.toString() + "}\n\n");

            outStream(outFile, "\\begin{document}\n\n");
            outStream(outFile, "\\maketitle\n\n");

            outFile.close();
        }

        template <typename T>
        static void sendOutput(const T &output, const bool ignoreFile, const RGB *color = nullptr, const DecimalFormat *decimalFormat = nullptr, const Alignment *alignment = nullptr, const Truncation *truncation = nullptr)
        {
            if (logLocation != "" && !ignoreFile)
            {
                outFile.open(logLocation, std::ios::app);

                outStream(outFile, output, decimalFormat, alignment, truncation);

                outFile.close();
            }
            else
            {
                if (color)
                    outStream(std::cout, getLogColor(*color));

                outStream(std::cout, output, decimalFormat, alignment, truncation);
            }
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

                std::string argument = truncation->getArgument();

                switch (truncation->getTruncation())
                {
                case Truncation::Truncate::NONE:
                    stream << argument;
                    break;
                case Truncation::Truncate::LEFT:
                    if (argLength >= formatLength)
                        stream << argument.substr(static_cast<size_t>(argLength - (argLength - formatLength)), static_cast<size_t>(argLength));

                    break;
                case Truncation::Truncate::RIGHT:
                    if (argLength >= formatLength)
                        stream << argument.substr(0, static_cast<size_t>(argLength - formatLength));

                    break;
                case Truncation::Truncate::CENTER:
                    if (argLength >= formatLength)
                        stream << argument.substr(static_cast<size_t>((argLength - formatLength) / 2), static_cast<size_t>(argLength - (argLength - formatLength) + (argLength & 1)));

                    break;
                default:
                    break;
                }
            }
            else
                stream << output;
        }

        template <class... Args>
        static void printer(const std::string &logMessage, const bool ignoreFile, const Args &...args)
        {
            std::vector<std::any> anyArgs = {args...};

            unsigned long argsLength = anyArgs.size();

            std::string timeString = "";

            time_t ttime = time(nullptr);

            tm *local_time = localtime(&ttime);

            setTimeFormat(timeString, 1, local_time);

            timeString += ":";

            setTimeFormat(timeString, 4, local_time);

            timeString += ":";

            setTimeFormat(timeString, 7, local_time);

            timeString += "] " + header + ": ";

            sendOutput(timeString, ignoreFile);

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
                            LG_FATAL("\n{0} is is greater than the provided amount of arguments in:\n\t{1}", true, splitArgs[0], logMessage);

                        printAtIndex(anyArgs, position, splitArgs[1], logMessage);
                    }
                    else
                        LG_FATAL("\n{0} is an invalid positional argument in:\n\t{1}", true, splitArgs[0], logMessage);
                }
                else
                    sendOutput(logMessage[static_cast<unsigned long>(i++)], ignoreFile);
            }

            if (logLocation == "")
            {
                sendOutput("\033[0m", ignoreFile);
                sendOutput("\n", ignoreFile);
            }
        }

        static void printAtIndex(const std::vector<std::any> &args, const unsigned long index, std::string &formatting, const std::string &logMessage)
        {
            DecimalFormat decimalFormat(formatting);

            Alignment alignment(formatting);

            Truncation truncation(formatting);

            if (args[index].type() == typeid(short))
            {
                short value = std::any_cast<short>(args[index]);

                handleFormats(value, std::to_string(value), decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(unsigned short))
            {
                unsigned short value = std::any_cast<unsigned short>(args[index]);

                handleFormats(value, std::to_string(static_cast<int>(value)), decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(int))
            {
                int value = std::any_cast<int>(args[index]);

                handleFormats(value, std::to_string(value), decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(unsigned int))
            {
                unsigned int value = std::any_cast<unsigned int>(args[index]);

                handleFormats(value, std::to_string(value), decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(long))
            {
                long value = std::any_cast<long>(args[index]);

                handleFormats(value, std::to_string(value), decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(unsigned long))
            {
                unsigned long value = std::any_cast<unsigned long>(args[index]);

                handleFormats(value, std::to_string(value), decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(float))
            {
                float value = std::any_cast<float>(args[index]);

                handleFormats(value, std::to_string(value), decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(double))
            {
                double value = std::any_cast<double>(args[index]);

                handleFormats(value, std::to_string(value), decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(std::string))
            {
                std::string value = std::any_cast<std::string>(args[index]);

                handleFormats(value, value, decimalFormat, alignment, truncation);
            }
            else if (args[index].type() == typeid(bool))
            {
                std::string value = std::any_cast<bool>(args[index]) ? "true" : "false";

                handleFormats(value, value, decimalFormat, alignment, truncation);
            }
            else if (std::strcmp(args[index].type().name(), "PKc") == 0)
            {
                const char *value = std::any_cast<const char *>(args[index]);

                std::string inString(value);

                handleFormats(value, inString, decimalFormat, alignment, truncation);
            }
            else
                LG_FATAL("\nArgument {0} is not an allowed type to be printed in:\n\t{1}", index, logMessage);
        }

        template <typename T>
        static void handleFormats(const T &value, const std::string &output, DecimalFormat &decimalFormat, Alignment &alignment, Truncation &truncation)
        {
            size_t length = output.length();

            alignment.setLength(length);

            truncation.setArgument(output);

            handlePrintAtIndex(value, decimalFormat, alignment, truncation);
        }

        template <typename T>
        static void handlePrintAtIndex(const T &val, const DecimalFormat &decimalFormat, const Alignment &alignment, const Truncation &truncation)
        {
            sendOutput(val, false, nullptr, &decimalFormat, &alignment, &truncation);
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
    };
}
