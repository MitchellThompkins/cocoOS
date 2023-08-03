#include "stdio.h"
#include "stdlib.h"
#include "platform.h"

#include "CppUTest/CommandLineTestRunner.h"

#include <vector>
#include <iostream>

#include "stdio.h"
#include "os_task.h"
#include "os_kernel.h"

int main(int argc, char *argv[])
{
    const uint16_t interval_ms = 250;

    platform_setup_timer(interval_ms);
    platform_enable_timer();

    int result = CommandLineTestRunner::RunAllTests(argc, argv);

    return result;
}

