#include "log.h"

int main()
{
    Logging::Log log;

    log.setHeader("LOGGER");
    log.setTimeFormatting("%H:%M:%S");
    // log.setLogInfo("./logs", "log.txt");

    std::string testString = "Hello World";

    LG_INFO("Hello World, {2} - {4} {1:0.2f} {0:0.2f}", 21, 6.226, "else", false, testString);

    return 0;
}