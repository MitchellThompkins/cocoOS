#include "CppUTestExt/MockSupport_c.h"

#include "os_msgqueue.h"

void os_msgQ_init( void )
{
    mock_c()->actualCall("os_msgQ_init");
}
