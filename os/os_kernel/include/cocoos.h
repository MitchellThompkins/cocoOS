#ifndef COCOOS_H
#define COCOOS_H

/** @file cocoos.h cocoOS kernel header file*/

#include <stdint.h>

#include "os_task.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NO_TID   255
#define NO_EVENT 255
#define NO_QUEUE 255
#define NO_SEM   255

//TODO(@mthompkins): Figure out if we need all these macros
#define OS_BEGIN  uint16_t os_task_state = task_internal_state_get(running_tid); switch ( os_task_state ) { case 0:

#define OS_END    os_task_kill(running_tid);\
                  running_tid = NO_TID;\
                  return;}


#define OS_SCHEDULE(ofs)    os_task_internal_state_set(running_tid, __LINE__+ofs);\
                            running_tid = NO_TID;\
                            return;\
                            case (__LINE__+ofs):




#define OS_WAIT_TICKS(x,y)	do {\
                                os_task_wait_time_set( running_tid, y, x );\
                                OS_SCHEDULE(0);\
                                 } while ( 0 )


#define task_open_macro() do{ if( !task_should_run_test( os_get_running_tid_test() ) ){return;} else{task_open_test();} } while(0)
#define task_wait_macro(x) do{ task_wait_test(x); return; } while(0)
#define task_close_macro(x) do{ task_close_test(); } while(0)

//TODO(@tthompkins): Consider making these static uint8_t private to
//os_kernel.c
extern uint8_t running_tid;
extern uint8_t last_running_task;
extern uint8_t running;


//TODO(@tthompkins): Consider removing this
#ifdef UNIT_TEST
//void os_run();
//void os_run_until_taskState(uint8_t taskId, TaskState_t state);
//TaskState_t os_get_task_state(uint8_t taskId);
//uint8_t os_get_running_tid(void);
#endif

#ifdef __cplusplus

}

#endif

#endif
