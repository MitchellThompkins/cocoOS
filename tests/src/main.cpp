#include "stdio.h"
#include "stdlib.h"
#include "platform.h"
#include "cocoos.h"

#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"

#include <vector>
#include <iostream>

#define N_TASKS  1
#define N_QUEUES 0
#define N_SEMAPHORES 0
#define N_EVENTS 0
#define ROUND_ROBIN 0
#define Mem_t uint32_t

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
    platform_setup_timer();
    platform_enable_timer();

    os_init();

    /* Create kernel objects */
    task_create( hello_task, NULL, 1, NULL, 0, 0 );


    os_start();

    for(volatile uint16_t a;;a++)
    {
        if(get_system_time() > 2)
        {
            break;
        }
    }

    int result = CommandLineTestRunner::RunAllTests(argc, argv);

    return result;
}

TEST_GROUP(TestClass)
{
  void setup()
  {
  }

  void teardown()
  {
  }
};

TEST(TestClass, Fail)
{
  CHECK_EQUAL(1,0);
}

TEST(TestClass, Pass)
{
  CHECK_EQUAL(1,1);
}
