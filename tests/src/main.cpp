#include <stdint.h>

#include "CppUTest/CommandLineTestRunner.h"

int main(int argc, char *argv[])
{
    int result = CommandLineTestRunner::RunAllTests(argc, argv);

    return result;
}

