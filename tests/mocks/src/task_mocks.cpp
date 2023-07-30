#include "CppUTestExt/MockSupport.h"

#include "mocks.hpp"

void os_assert()
{
    mock().actualCall("os_assert");
}
