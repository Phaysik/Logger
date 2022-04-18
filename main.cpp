#include "log.h"

int main()
{
    Logging::Log log;

    log.setHeader("LOGGER");
    log.setTimeFormatting("%H:%M:%S");
    // log.setLogInfo("./logs", "log.txt");

    std::string testString = "testing String";
    std::string testString2 = "truncation";

    LG_INFO("Hello World, {2:=12} - {4:>20} {5:-4!}{5:6!} {5:=8!} {1:0.2f} {0:0.1f}", 21.56, 6.226, "else", false, testString, testString2);

    return 0;
}