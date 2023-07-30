#include "stdio.h"
#include "stdlib.h"
#include "platform.h"
#include "cocoos.h"

#include "CppUTest/CommandLineTestRunner.h"

#include <vector>
#include <iostream>

#define N_TASKS  1
#define N_QUEUES 0
#define N_SEMAPHORES 0
#define N_EVENTS 0
#define ROUND_ROBIN 0
#define Mem_t uint32_t

int main(int argc, char *argv[])
{
    const uint16_t interval_ms = 250;

    platform_setup_timer(interval_ms);
    platform_enable_timer();

    int result = CommandLineTestRunner::RunAllTests(argc, argv);

    return result;
}

