#include "log.h"

int main()
{
    Logging::Log log;

    log.setLogInfo("./logs", "main", "Matthew Moore");
    log.setHeader("LOGGER");
    log.setTimeFormatting("%H:%S:%M");

    std::string testString = "testing String";
    std::string testString2 = "truncation";

    LG_INFO("Hello World, {2:=12} - {4:>20} {5:-4!}{5:6!} {3:=2!} {1:0.2f} {0:0.1f}", false, 21.56, 6.226, "else", false, testString, testString2);
    LG_WARN("Please", false);
    LG_TEST_SUCCESS("Success", false);

    log.setHeader("NEW LOGGER");
    log.setTimeFormatting("%M:%H:%S");

    LG_WARN("Failure", false);
    LG_INFO("HELP", false);
    LG_TEST_FAIL("FAILED", false);

    return 0;
}