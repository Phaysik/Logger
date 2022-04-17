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
        static void sendOutput(const T &output, const bool decimalFormat = false, const int precision = 0)
        {
            if (logLocation != "")
            {
                outFile.open(logLocation, std::ios::app);

                if (decimalFormat)
                    outFile << std::fixed << std::setprecision(precision) << output;
                else
                    outFile << output;

                outFile.close();
            }
            else
                std::cout << output;
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
            std::regex decimalRegex("^0?.\\d*f$");

            bool decimalMatch = false;

            int decimalPrecision = 0;

            if (std::regex_match(formatting, decimalRegex))
            {
                formatting.pop_back();

                decimalMatch = true;

                decimalPrecision = static_cast<int>(std::stof(formatting) * 10);
            }

            if (args[index].type() == typeid(short))
                handlePrintAtIndex(std::any_cast<short>(args[index]), decimalMatch, decimalPrecision);
            else if (args[index].type() == typeid(unsigned short))
                handlePrintAtIndex(std::any_cast<unsigned short>(args[index]), decimalMatch, decimalPrecision);
            else if (args[index].type() == typeid(int))
                handlePrintAtIndex(std::any_cast<int>(args[index]), decimalMatch, decimalPrecision);
            else if (args[index].type() == typeid(unsigned int))
                handlePrintAtIndex(std::any_cast<unsigned int>(args[index]), decimalMatch, decimalPrecision);
            else if (args[index].type() == typeid(long))
                handlePrintAtIndex(std::any_cast<long>(args[index]), decimalMatch, decimalPrecision);
            else if (args[index].type() == typeid(unsigned long))
                handlePrintAtIndex(std::any_cast<unsigned long>(args[index]), decimalMatch, decimalPrecision);
            else if (args[index].type() == typeid(float))
                handlePrintAtIndex(std::any_cast<float>(args[index]), decimalMatch, decimalPrecision);
            else if (args[index].type() == typeid(double))
                handlePrintAtIndex(std::any_cast<double>(args[index]), decimalMatch, decimalPrecision);
            else if (args[index].type() == typeid(std::string))
                handlePrintAtIndex(std::any_cast<std::string>(args[index]), decimalMatch, decimalPrecision);
            else if (args[index].type() == typeid(bool))
                handlePrintAtIndex(std::any_cast<bool>(args[index]), decimalMatch, decimalPrecision);
            else if (std::strcmp(args[index].type().name(), "PKc") == 0)
                handlePrintAtIndex(std::any_cast<const char *>(args[index]), decimalMatch, decimalPrecision);
            else
                LG_FATAL("\nArgument {0} is not an allowed type to be printed in:\n\t{1}", index, logMessage);
        }

        template <typename T>
        static void handlePrintAtIndex(const T &val, const bool decimalMatch, const int precision)
        {
            if (decimalMatch)
                sendOutput(val, decimalMatch, precision);
            else
                sendOutput(val);
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
