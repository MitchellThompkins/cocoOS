#include <stdint.h>

#include "CppUTest/CommandLineTestRunner.h"

#include "CppUTest/TestOutput.h"

int main(int argc, char *argv[])
{
    int result = CommandLineTestRunner::RunAllTests(argc, argv);

    return result;
}

