#include "CppUTestExt/MockSupport_c.h"

#include "cocoos.h"
#include "os_kernel.h"
#include "os_event.h"
#include "os_msgqueue.h"
#include "os_port.h"
#include "os_sem.h"
#include "os_task.h"

uint8_t running;
uint8_t running_tid;
uint8_t last_running_task;

void os_init( void )
{
    running_tid = NO_TID;
    last_running_task = NO_TID;
    running = 0;
    mock_c()->actualCall("os_init");
}

void os_start(void)
{
    running = 1;
    mock_c()->actualCall("os_start");
}

void os_tick( void )
{}

void os_sub_tick( uint8_t id )
{}

void os_sub_nTick( uint8_t id, uint32_t nTicks )
{}

uint8_t os_running(void)
{
    return running;
}

uint8_t os_get_running_tid(void)
{
    return running_tid;
}

Sem_t sem_bin_create( uint8_t initial )
{}

Sem_t sem_counting_create( uint8_t max, uint8_t initial )
{}

Evt_t event_create( void )
{}

uint8_t event_signaling_taskId_get( Evt_t ev )
{}

void os_cbkSleep( void )
{}

