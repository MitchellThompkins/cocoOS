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
//
#define OS_BEGIN     switch ( task_internal_state_get( os_get_running_tid() ) ) { case 0:

#define OS_END       os_task_kill( os_get_running_tid() );\
                     os_free_tid();\
                     return; }

#define OS_SCHEDULE  os_task_internal_state_set( os_get_running_tid(), __LINE__ );\
                     os_free_tid();\
                     return;\
                     case (__LINE__):


#define OS_WAIT_TICKS(time,id)  do { os_task_wait_time_set( os_get_running_tid(), id, time );\
                                OS_SCHEDULE; } while (0)

//TODO(@tthompkins): Consider making these static uint8_t private to
//os_kernel.c
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
