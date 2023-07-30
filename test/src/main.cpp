#include "stdio.h"
#include "stdlib.h"
#include "platform.h"

#include "CppUTest/CommandLineTestRunner.h"
#include "CppUTest/TestHarness.h"

#include <vector>
#include <iostream>

int main(int argc, char *argv[])
{
    printf("hello world\n");

    platform_setup_timer();
    platform_enable_timer();

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
