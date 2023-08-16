#include <stdint.h>

#include "CppUTest/CommandLineTestRunner.h"

int main(void)
{
    // TODO(@mthompkins): Consider taking these in at the commandline, although
    // this does give us control over what arguments are passed.
    int argc = 3;
    const char *argv[argc] =
    {
        "test_name",
        "-ojunit",
        "-v",
    };

    int result = CommandLineTestRunner::RunAllTests(argc, argv);

    return result;
}

