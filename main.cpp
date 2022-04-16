#include "log.h"

int main()
{
    Logging::Log log;

    log.setHeader("LOGGER");
    log.setTimeFormatting("%H:%H:%S");

    std::string testString = "Hello World";

    LG_INFO("Hello World, {w} - {4} {1:0.2f} {0:0.2f}", 21, 6.226, "else", false, testString);

    return 0;
}