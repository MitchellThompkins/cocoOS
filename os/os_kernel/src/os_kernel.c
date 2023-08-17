#include <stdint.h>

#include "cocoos.h"
#include "os_kernel.h"
#include "os_event.h"
#include "os_msgqueue.h"
#include "os_port.h"
#include "os_sem.h"
#include "os_task.h"

static void os_schedule( void );

uint8_t running_tid;
uint8_t last_running_task;
uint8_t running;

/*********************************************************************************/
/*  void os_init()

*   Initializes the scheduler.
*   @return None.
*   @remarks \b Usage: @n Should be called early in system setup, before starting the task
*   execution
*
*   @code
*   int main(void) {
*   system_init();
*   os_init();
*
*   }
*   @endcode
* */
/*********************************************************************************/
void os_init( void )
{
    running_tid = NO_TID;
    last_running_task = NO_TID;
    running = 0;
    os_sem_init();
    os_event_init();
    os_msgQ_init();
    os_task_init();
}


static void os_schedule( void )
{
    running_tid = NO_TID;

#if (ROUND_ROBIN)
    // Find next ready task
    running_tid = next_ready_task();
#else
    // Find the highest prio task ready to run
    running_tid = highest_prio_ready_task();
#endif

    if ( running_tid != NO_TID )
    {
        task_run();
    }
    else
    {
        os_cbkSleep();
    }
}


/*********************************************************************************/
/*  void os_start()
*   Starts the task scheduling
*
*   @return None.
*   @remarks \b Usage: @n Should be the last line of main.
*
*   @code
*   int main(void)
*   {
*       system_init();
*       os_init();
*       task_create( myTaskProc, 1, NULL, 0, 0 );
*
*       os_start();
*       return 0;
*   }
*   @endcode
*/
/*********************************************************************************/
void os_start( void )
{
    running = 1;
    os_enable_interrupts();

    for(;;)
    {
        os_schedule();
    }
}


/*********************************************************************************/
/*  void os_tick()                                              *//**
*
*   Tick function driving the kernel
*
*   @return None.
*   @remarks \b Usage: @n Should be called periodically. Preferably from the clock tick ISR.
*
*   @code
*   ISR(SIG_OVERFLOW0) {
*     ...
*     os_tick();
*   }
*
*   @endcode
*
*/
/*********************************************************************************/
void os_tick( void )
{
    // Master clock tick
    task_tick( 0, 1 );
}


/*********************************************************************************/
/*  void os_sub_tick( id )                                              *//**
*
*   Tick function driving the sub clocks
*
*   @param id sub clock id, allowed range 1-255
*
*   @return None.
*   @remarks \b Usage: @n Could be called at any desired rate to trigger timeouts.
*   Called from a task or from an interrupt ISR.
*
*   @code
*   ISR(SIG_OVERFLOW0) {
*     ...
*     os_sub_tick(2);
*   }
*
*   @endcode
*
*/
/*********************************************************************************/
void os_sub_tick( uint8_t id )
{
    /* Sub clock tick */
    if ( id != 0 ) {
        task_tick( id, 1 );
    }
}


/*********************************************************************************/
/*  void os_sub_nTick( id, nTicks )
*
*   Tick function driving the sub clocks. Increments the tick count with nTicks.
*
*   @param id sub clock id, allowed range 1-255.
*   @param nTicks increment size, 16 bit value.
*   @return None.
*   @remarks \b Usage: @n Could be called at any desired rate to trigger timeouts.
*   Called from a task or from an interrupt ISR.
*
*   @code
*   ISR(SIG_OVERFLOW0) {
*     ...
*     os_sub_nTick( 2, 10 );
*   }
*
*   @endcode
*
*/
/*********************************************************************************/
void os_sub_nTick( uint8_t id, uint32_t nTicks )
{
    // Sub clock tick
    if ( id != 0 )
    {
        task_tick( id, nTicks );
    }
}


uint8_t os_running( void )
{
    return running;
}

uint8_t os_get_running_tid(void)
{
    return running_tid;
}
