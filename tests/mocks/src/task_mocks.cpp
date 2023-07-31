#include "CppUTestExt/MockSupport.h"

#include "mocks.hpp"
#include "stdio.h"

void os_assert()
{
    printf("called mock");
    mock().actualCall("os_assert");
}
