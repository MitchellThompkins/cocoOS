#include "CppUTestExt/MockSupport_c.h"

#include "os_assert.h"

bool os_on_assert(const char* file, uint16_t line, const char* expr)
{
    mock_c()->actualCall("os_on_assert")->returnValue();
}
