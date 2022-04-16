#include <string>
#include <iostream>
#include <ctime>
#include <vector>
#include <any>
#include <typeinfo>
#include <sstream>
#include <regex>
#include <iomanip>

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
                std::cout << std::endl;
                newLogMessage.erase(0, 1);
            }

            std::cout << "\033[38;2;255;255;255m[";

            printer(newLogMessage, args...);
        }

        template <class... Args>
        static void warn(const std::string &logMessage, const Args &...args)
        {
            std::string newLogMessage = logMessage;

            if (newLogMessage[0] == '\n')
            {
                std::cout << std::endl;
                newLogMessage.erase(0, 1);
            }

            std::cout << "\033[38;2;232;129;26m[";

            printer(newLogMessage, args...);
        }

        template <class... Args>
        static void fatal(const std::string &logMessage, const Args &...args)
        {
            std::string newLogMessage = logMessage;

            if (newLogMessage[0] == '\n')
            {
                std::cout << std::endl;
                newLogMessage.erase(0, 1);
            }

            std::cout << "\033[38;2;217;28;28m[";

            printer(newLogMessage, args...);

            exit(1);
        }

        template <class... Args>
        static void printer(const std::string &logMessage, const Args &...args)
        {
            std::vector<std::any> anyArgs = {args...};

            unsigned long argsLength = anyArgs.size();

            time_t ttime = time(nullptr);

            tm *local_time = localtime(&ttime);

            std::cout << local_time->tm_hour << ":" << local_time->tm_min << ":" << local_time->tm_sec << "]: " << header << ": ";

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
                            std::cout << "ERROR: " << position << " is out of bounds"; // TODO Make this a LG_FATAL

                        printAtIndex(anyArgs, position, splitArgs[1]);
                    }
                    else
                        LG_FATAL("\n{0} is an invalid positional argument in:\n\t{1}", splitArgs[0], logMessage);
                }
                else
                {
                    std::cout << logMessage[static_cast<unsigned long>(i++)];
                }
            }

            std::cout << ".\033[0m" << std::endl;
        }

        static void printAtIndex(const std::vector<std::any> &args, const unsigned long index, std::string &formatting)
        {
            std::regex decimalRegex("^0?.\\d*f$");

            if (std::regex_match(formatting, decimalRegex))
            {
                formatting.pop_back();
                std::cout << std::fixed << std::setprecision(static_cast<int>(std::stof(formatting) * 10));
            }

            if (args[index].type() == typeid(int))
                std::cout
                    << std::any_cast<int>(args[index]);
            else if (args[index].type() == typeid(float))
                std::cout << std::any_cast<float>(args[index]);
            else if (args[index].type() == typeid(double))
                std::cout << std::any_cast<double>(args[index]);
            else if (args[index].type() == typeid(std::string))
                std::cout << std::any_cast<std::string>(args[index]);
            else if (args[index].type() == typeid(bool))
                std::cout << std::any_cast<bool>(args[index]);
            else if (std::strcmp(args[index].type().name(), "PKc") == 0)
                std::cout << std::any_cast<const char *>(args[index]);
            else
                std::cout << "invalid argument"; // TODO Make this a LG_FATAL
        }

        static void argSplitter(const std::string &argument, std::string *argArray, int &index)
        {
            // Get the argument within the braces
            std::string enclosed = "";

            while (argument[static_cast<unsigned long>(++index)] != '}')
                enclosed += argument[static_cast<unsigned long>(index)];

            index++;

            // Split the argument on :

            char delimiter = ':';

            // If : not found
            if (enclosed.find(delimiter) == std::string::npos)
            {
                argArray[0] = enclosed;
                argArray[1] = "";
            }
            else
            {
                size_t pos = 0;
                std::string token;

                std::stringstream ss(enclosed);

                while (std::getline(ss, token, delimiter))
                    argArray[pos++] = token;
            }
        }

        void setTimeFormatting(const std::string &format);

        void setHeader(const std::string &header);

    private:
        static std::string timeFormatting;
        static std::string header;
    };
}
