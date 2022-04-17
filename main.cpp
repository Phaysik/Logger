#include "log.h"

int main()
{
    Logging::Log log;

    log.setHeader("LOGGER");
    log.setTimeFormatting("%H:%M:%S");
    // log.setLogInfo("./logs", "log.txt");

    std::string testString = "testing String";

    LG_INFO("Hello World, {2:=9} - {4:>20} {1:0.2f} {0:0.1f}", 21.56, 6.226, "else", false, testString);

    return 0;
}