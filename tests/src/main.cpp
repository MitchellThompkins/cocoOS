#include "stdio.h"
#include "stdlib.h"
#include "platform.h"

#include "CppUTest/CommandLineTestRunner.h"

#include <vector>
#include <iostream>

#include "stdio.h"
#include "os_task.h"
#include "os_kernel.h"

static void hello_task(void)
{
    task_open();

    for(;;)
    {
        printf("hello world\n");
        task_wait( 20 );
    }

    task_close();
}

int main(int argc, char *argv[])
{
    const uint16_t interval_ms = 250;

    platform_setup_timer(interval_ms);
    platform_enable_timer();

    os_init();
    task_create( hello_task, NULL, 1, NULL, 0, 0 );
    os_start();

    int result = CommandLineTestRunner::RunAllTests(argc, argv);

    return result;
}

