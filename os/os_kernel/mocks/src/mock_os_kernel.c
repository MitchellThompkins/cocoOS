#include "CppUTestExt/MockSupport_c.h"

#include "os_event.h"
#include "os_kernel.h"
#include "os_msgqueue.h"
#include "os_port.h"
#include "os_reentry.h"
#include "os_sem.h"
#include "os_task.h"

static uint8_t running;
static uint8_t running_tid;

void os_init(void)
{
    running_tid = NO_TID;
    running = 0;

    mock_c()->actualCall("os_init");
}

void os_start( const uint32_t tick_limit )
{
    running = 1;

    mock_c()->actualCall("os_start");
}

void os_tick( void )
{
    mock_c()->actualCall("os_tick");
}

void os_sub_tick( uint8_t id )
{
    mock_c()->actualCall("os_sub_tick");
}

void os_sub_nTick( uint8_t id, uint32_t nTicks )
{
    mock_c()->actualCall("os_sub_nTick");
}

uint8_t os_running(void)
{
    mock_c()->actualCall("os_running");
    return running;
}

uint16_t os_get_running_tid(void)
{
    const int v =
        (int)mock_c()->getData("running_tid_from_get").value.intValue;

    mock_c()->actualCall("os_get_running_tid");

    return v;
}

void os_free_tid(void)
{
    mock_c()->actualCall("os_free_tid");
}

void os_cbkSleep( void )
{
    mock_c()->actualCall("os_cbkSleep");
}

