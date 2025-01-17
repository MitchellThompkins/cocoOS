#include <stdlib.h>

#include "os_assert.h"
#include "os_kernel.h"
#include "os_reentry.h"
#include "os_task.h"
#include "os_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO(@mthompkins): Change this to better name
struct tcb
{
    taskproctype taskproc;
    TaskState_t state;       ///< current runstate
    TaskState_t savedState;  ///< saves the task state when suspending
    uint16_t internal_state; ///< is set when calling OS_YIELD
    uint32_t time;
    uint8_t tid;
    uint8_t prio;
    Sem_t semaphore;
    MsgQ_t msgQ;
    MsgQ_t waitQ;            ///< The queue the task is waiting for (post or receive)
    Evt_t msgChangeEvent;    ///< The change event of the message queue the task is waiting for
    uint8_t msgResult;       ///< The result of msg_receive or msg_post
    bool waitSingleEvent;
    uint8_t clockId;
    EventQueue_t eventQueue;
    void *data;
};


static void task_suspended_set( uint8_t tid );
static void task_waiting_time_set( uint8_t tid );
static void task_waiting_event_set( tcb *task );
static void task_waiting_event_timeout_set( tcb *task );
static bool os_task_wait_queue_empty( uint8_t tid );
static void task_ready_set( uint8_t tid );

static tcb task_list[ N_TASKS ];
static uint8_t nTasks = 0;

static uint16_t last_running_task = 0;

void os_task_init( void )
{
    last_running_task = 0;

    nTasks = 0;
    tcb *task;

    for ( uint8_t i = 0; i < N_TASKS; ++i )
    {
        task = &task_list[i];
        task->clockId = 0xff; //TODO(@mthompkins): make this an enum
        task->internal_state = 0xff; //TODO(@mthompkins): make this an enum
        task->msgQ = 0;
        task->waitQ = 0;
        task->msgChangeEvent = 0;
        task->msgResult = 0;
        task->prio = 0;
        task->savedState = SUSPENDED;
        task->semaphore = NO_SEM;
        task->state = SUSPENDED;
        task->taskproc = 0;
        task->tid = NO_TID;
        task->time = 0;
        task->waitSingleEvent = false;

        for ( uint8_t j = 0; j < sizeof( task->eventQueue.eventList); j++ )
        {
            //TODO(@mthompkins): make this an enum
            task->eventQueue.eventList[j] = 0xff;
        }
        task->data = 0;
    }
}


/************************************************************** *******************/
/*  uint8_t os_task_create( taskproctype taskproc, void *data, uint8_t prio, Msg_t *msgPool, uint8_t poolSize, uint16_t msgSize )
*   
*   Creates a task scheduled by the os. The task is put in the ready state.
*
*   @param taskproc Function pointer to the task procedure.
*   @param data [optional] Pointer to task data
*   @param prio Task priority on a scale 0-255 where 0 is the highest priority.
*   @param msgPool [optional] Pointer to the message pool, containing messages. Ignored if poolSize is 0.
*   @param poolSize [optional] Size, in nr of messages, of the message pool. Set to 0 if no message pool needed for the task
*   @param msgSize [optional] Size of the message type held in the message queue
*   @return Task id of the created task.
*
*   @remarks \b Usage: @n Should be called early in system setup, before starting the task
*   execution. Only one task per priority level is allowed.
*
*   @code
*   static uint8_t taskId;
*   static Msg_t msgpool_1[ POOL_SIZE ];

int main(void) {
    system_init();
    os_init();
    taskId = os_task_create( myTaskProc, 0, 1, msgpool_1, POOL_SIZE, sizeof(Msg_t) );
    ...
}
@endcode
*/
/*********************************************************************************/
uint8_t os_task_create(
        taskproctype taskproc,
        void *data,
        uint8_t prio,
        Msg_t *msgPool,
        uint8_t poolSize,
        uint16_t msgSize )
{

    uint8_t taskId;
    tcb *task;

    os_assert_with_return( os_running() == 0, 1 );
    os_assert_with_return( nTasks < N_TASKS, 1 );
    os_assert_with_return( taskproc != NULL, 1 );

    taskId = nTasks;

    /* Check that no other task has the same prio */
    while ( taskId != 0 )
    {
        --taskId;
        os_assert_with_return( task_list[ taskId ].prio != prio, 1 );
    }

    task = &task_list[ nTasks ];

    task->tid = nTasks;
    task->prio = prio;
    task->state = READY;
    task->savedState = READY;
    task->semaphore = 0;
    task->internal_state = 0;
    task->taskproc = taskproc;
    task->waitSingleEvent = false;
    task->time = 0;

    if ( poolSize > 0 )
    {
        task->msgQ = os_msgQ_create( msgPool, poolSize,
                                     msgSize, task->tid );
    }
    else
    {
        task->msgQ = NO_QUEUE;
    }

    task->data = data;
    os_task_clear_wait_queue( nTasks );

    nTasks++;
    return task->tid;
}


TaskState_t task_state_get( uint8_t tid )
{
    os_assert_with_return( tid < nTasks, 1 );
    return task_list[ tid ].state;
}




/*********************************************************************************/
/*  void os_task_kill( uint8_t task_id )                                   *//**
*   
*   Puts the task associated with the specified id in the killed state. 
*   A killed task, cannot be resumed.
*
*   @param task_id id of the task.
*   @return None
*
*   @code

static uint8_t taskId;

static void waitingTask(void)
{
    task_open();

    event_wait( event );

    if ( event_signaling_taskId_get( event ) == taskId ) {
        os_task_kill( taskId );
    }

	task_close();

}


static void signalingTask1(void)
{
	task_open();	
	
	task_wait( 900 );
       
    event_signal(event);

	task_close();

}

int main() {
   ...
   taskId = os_task_create(signalingTask1, ...);
   ...
}
*		@endcode
*       
*/
/*********************************************************************************/
void os_task_kill( const uint8_t tid )
{
    os_assert( tid < nTasks );
    task_list[ tid ].state = KILLED;
}


/*********************************************************************************/
/*  void task_get_data()                                   *//**
*
*   Gets a pointer to the data structure associated with the task
*
*   @return pointer to task data
*
*   @code


static uint32_t taskData;

static void taskProc(void)
{
  task_open();
  for (;;) {
    task_wait( 100 );
    uint32_t *data = (uint32_t*)task_get_data();
    *data++;
  }
  task_close();
}

int main() {
   ...
   os_task_create(taskProc, (void*)&taskData, ...);
   ...
}
*   @endcode
*
*/
/*********************************************************************************/
void *task_get_data()
{
    const uint16_t tid = os_get_running_tid();
    return task_list[ tid ].data;
}

// Finds the task with highest prio that is ready to run
// Used with priority based scheduling
uint8_t highest_prio_ready_task( void )
{
    tcb *task;
    uint8_t highest_prio_task = NO_TID;
    uint8_t highest_prio = 255;
    TaskState_t state;
    uint8_t prio;

    for( uint16_t i = 0; i != nTasks; ++i )
    {
        task = &task_list[i];
        prio = task->prio;
        state = task->state;

        if ( READY == state )
        {
            if ( prio < highest_prio )
            {
                highest_prio = prio;
                highest_prio_task = i;
            }
        }
    }

    return highest_prio_task;
}


// Finds the next ready task - used when ROUND_ROBIN is defined
uint8_t next_ready_task( void )
{
    uint16_t index;
    uint8_t found;
    uint8_t nChecked;

    if ( NO_TID == last_running_task )
    {
        index = 0;
    }
    else
    {
        index = last_running_task + 1;
        if ( index >= nTasks )
        {
            index = 0;
        }
    }

    found = 0;
    nChecked = 0;

    do
    {
        if ( READY == task_list[ index ].state )
        {
            last_running_task = index;
            found = 1;
            break;
        }

        ++index;
        if ( index == nTasks )
        {
            index = 0;
        }
    } while ( ++nChecked != nTasks );

    if ( !found )
    {
        last_running_task = NO_TID;
    }

    return last_running_task;
}

// If using prio based schedule finds the task with highest prio waiting for
// sem, and makes it ready to run
// If using round robin based schedule finds the task which has been waiting on
// a semaphore the longest and makes it ready to run
void os_task_release_waiting_task( Sem_t sem )
{
#if (ROUND_ROBIN)
    uint32_t longestWaitTime = 0;
    uint8_t lastCheckedTask = NO_TID;
#else
    uint8_t highestPrio = 255;
#endif

    uint8_t tid;
    uint8_t foundTask = NO_TID;
    uint8_t taskIsWaitingForThisSemaphore;
    tcb *task;

    for ( tid = 0; tid != nTasks; ++tid )
    {
        task = &task_list[ tid ];
        taskIsWaitingForThisSemaphore = (( task->state == WAITING_SEM ) && ( task->semaphore == sem ) );

        if ( taskIsWaitingForThisSemaphore == 1 )
        {
#if (ROUND_ROBIN)
            /* Release the task that has waited longest */
            lastCheckedTask = tid;
            if ( task->time > longestWaitTime )
            {
                longestWaitTime = task->time;
                foundTask = tid;
            }
#else
            /* Release the highest prio task */
            if ( task->prio < highestPrio )
            {
                highestPrio = task->prio;
                foundTask = tid;
            }
#endif
        }
    }

    /* We have found a waiting task. */
#if (ROUND_ROBIN)
    if (( longestWaitTime == 0 ) && ( NO_TID != lastCheckedTask ))
    {
        /* All waiting tasks had waiting time 0 -> release the last task */
        foundTask = lastCheckedTask;
    }
#endif
    if ( NO_TID != foundTask )
    {
        task_list[ foundTask ].state = READY;
    }
}


// Checks if any task is waiting for this semaphore
uint8_t os_task_waiting_this_semaphore( Sem_t sem )
{
    uint8_t tid;
    tcb *task;
    uint8_t taskIsWaitingForThisSemaphore;
    uint8_t result = 0;

    for ( tid = 0; tid != nTasks; ++tid )
    {
        task = &task_list[ tid ];
        taskIsWaitingForThisSemaphore = (( task->state == WAITING_SEM ) && ( task->semaphore == sem ) );

        if ( taskIsWaitingForThisSemaphore == 1 )
        {
            result = 1;
            break;
        }
    }

    return result;
}

// TODO(@mthompkins): This is a thin wrapper around a call to this function, so
// I plan to remove it
/* Sets the task to ready state */
void os_task_ready_set( uint8_t tid )
{
    os_assert( tid < nTasks );
    task_ready_set( tid );
}


void os_task_suspend( uint8_t tid )
{
    TaskState_t state;

    os_assert( tid < nTasks );

    state = task_list[ tid ].state;

    if (( state != KILLED ) && ( state != SUSPENDED ))
    {
        // If a task is waiting for a semaphore when being suspended, there is
        // a risk that the semaphore will be signaled while the task is
        // suspended, and if the task is then resumed it could hang if the
        // semaphore is not signaled again. Therefore the task is reset when it
        // is resumed.
        if ( WAITING_SEM == state ) {
            task_list[ tid ].savedState = READY;
            task_list[ tid ].internal_state = 0;
        }
        else {
            task_list[ tid ].savedState = state;
        }

        task_suspended_set( tid );
    }
}


void os_task_resume( uint8_t tid )
{
    os_assert( tid < nTasks );

    if ( task_list[ tid ].state == SUSPENDED ) {
        task_list[ tid ].state = task_list[ tid ].savedState;
    }
}


uint8_t os_task_prio_get( uint8_t tid )
{
    os_assert_with_return( tid < nTasks, 1 );
    return task_list[ tid ].prio;

}


// Clears the event wait queue of a task
void os_task_clear_wait_queue( uint8_t tid )
{
    tcb *task = &task_list[ tid ];

    task->waitSingleEvent = false;
    uint8_t event = EVENT_QUEUE_SIZE;

    do
    {
        --event;
        task->eventQueue.eventList[ event ] = 0;
    } while ( event != 0 );

}


// Checks if a tasks event wait queue is empty or not
static bool os_task_wait_queue_empty( uint8_t tid )
{
    uint8_t event = EVENT_QUEUE_SIZE;

    // Scan through each major index of eventList, and if it is zero, then
    // there are events
    do
    {
        --event;
        if( task_list[ tid ].eventQueue.eventList[ event ] != 0 )
        {
            return false;
        }
    } while( event != 0 );

    return true;
}


void os_task_wait_time_set( const uint8_t tid,
                            const uint8_t id,
                            const uint32_t time )
{
    os_assert( tid < nTasks );
    os_assert( time > 0 );

    task_list[ tid ].clockId = id;
    task_list[ tid ].time = time;
    task_waiting_time_set( tid );
}


void os_task_wait_event( uint8_t tid,
                         Evt_t eventId,
                         bool waitSingleEvent,
                         uint32_t timeout )
{
    os_assert( tid < nTasks );

    tcb *task = &task_list[ tid ];

    const uint8_t eventListIndex = eventId / 8;
    const uint8_t shift = eventId & 0x07;

    task->eventQueue.eventList[ eventListIndex ] |= (1 << shift);
    task->waitSingleEvent = waitSingleEvent;

    if( timeout != 0 )
    {
        // Waiting for an event with timeout and with clockId = 0 master clock
        os_task_wait_time_set( tid, 0, timeout );

        task_waiting_event_timeout_set( task );
    }
    else
    {
        task_waiting_event_set( task );
    }
}


void task_tick( const uint8_t clockId, const uint32_t tickSize )
{
    // Search all tasks and decrement time for waiting tasks
    for( uint8_t i = 0; i != nTasks; ++i )
    {
        TaskState_t state;
        state = task_list[i].state;

        if( (state == WAITING_TIME) || (state == WAITING_EVENT_TIMEOUT) )
        {
            // Found a waiting task, is it ready?
            if( task_list[i].clockId == clockId )
            {
                if ( task_list[i].time <= tickSize )
                {
                    task_list[i].time = 0;

                    if( state == WAITING_EVENT_TIMEOUT )
                    {
                        os_task_clear_wait_queue(i);
                    }

                    task_ready_set(i);
                }
                else
                {
                    task_list[i].time -= tickSize;
                }
            }
        }
        else if( state ==  WAITING_SEM )
        {
            //TODO(@mthompkins): Make sure this handles overflow properly
            task_list[i].time++;
        }

        // If the task has a message queue, decrement the delayed message timers
        if( clockId == 0 )
        {
            if( task_list[i].msgQ != NO_QUEUE )
            {
                os_msgQ_tick( task_list[i].msgQ );
            }
        }
    }
}


void os_task_signal_event( const Evt_t eventId )
{
    const uint8_t eventListIndex = eventId / 8;
    const uint8_t shift = eventId & 0x07;

    for( uint8_t i = 0; i != nTasks; i++ )
    {
        const TaskState_t state = task_list[ i ].state;

        const bool taskWaitStateOK =
            (( state == WAITING_EVENT )
            || ( state == WAITING_EVENT_TIMEOUT ));

        const uint8_t taskWaitingForEvent =
            task_list[ i ].eventQueue.eventList[eventListIndex]
            & (1<<shift);

        if( taskWaitingForEvent && taskWaitStateOK )
        {
            // Mark the task as no longer waiting for the event
            task_list[ i ].eventQueue.eventList[eventListIndex]
                &= ~(1<<shift);

            if( task_list[ i ].waitSingleEvent ||
                os_task_wait_queue_empty( i ) )
            {
                os_task_clear_wait_queue( i );
                task_ready_set( i );
            }
        }
    }
}


// Runs the next task ready for execution.
// Assumes running_tid has been assigned.
void task_run( void )
{
    const uint16_t tid = os_get_running_tid();
    os_assert( tid < nTasks );
    task_list[ tid ].taskproc();
}


uint16_t task_internal_state_get( uint8_t tid )
{
    return task_list[ tid ].internal_state;
}


void os_task_internal_state_set( const uint8_t tid, const uint16_t state )
{
    task_list[ tid ].internal_state = state;
}


MsgQ_t os_task_msgQ_get( const uint8_t tid )
{
    return task_list[ tid ].msgQ;
}

void os_task_set_wait_queue(uint8_t tid, MsgQ_t queue) {
  task_list[ tid ].waitQ = queue;
}

MsgQ_t os_task_get_wait_queue(uint8_t tid) {
  return task_list[ tid ].waitQ;
}

void os_task_set_change_event(uint8_t tid, Evt_t event) {
  task_list[ tid ].msgChangeEvent = event;
}

Evt_t os_task_get_change_event(uint8_t tid) {
  return task_list[ tid ].msgChangeEvent;
}

void os_task_set_msg_result(uint8_t tid, uint8_t result) {
  task_list[ tid ].msgResult = result;
}

uint8_t os_task_get_msg_result(uint8_t tid) {
  return task_list[ tid ].msgResult;
}

// Use this to differentiate between event timeout or not
uint32_t os_task_timeout_get(uint8_t tid)
{
  return task_list[ tid ].time;
}

// Sets the task to wait for semaphore state
void task_wait_sem_set( uint8_t tid, Sem_t sem )
{
    os_assert( tid < nTasks );

    // The time is ticked to measure waiting time
    task_list[ tid ].time = 0;
    task_list[ tid ].state = WAITING_SEM;
    task_list[ tid ].semaphore = sem;
}


void task_ready_set( uint8_t tid )
{
    os_assert( tid < nTasks );
    task_list[ tid ].state = READY;
}


void task_suspended_set( uint8_t tid )
{
    task_list[ tid ].state = SUSPENDED;
}


static void task_waiting_time_set( uint8_t tid )
{
    task_list[ tid ].state = WAITING_TIME;
}


static void task_waiting_event_set( tcb *task )
{
    task->state = WAITING_EVENT;
}


static void task_waiting_event_timeout_set( tcb *task )
{
    task->state = WAITING_EVENT_TIMEOUT;
}


bool task_should_run_test(const uint16_t id)
{
    const uint8_t state = task_internal_state_get(id);
    return state==0 || state==99;
}

bool task_is_killed(const uint16_t id)
{
    const uint8_t state = task_internal_state_get(id);
    return state==KILLED;
}

void task_set_no_running_task(void)
{
    last_running_task = NO_TID;
}

#ifdef __cplusplus
}
#endif
