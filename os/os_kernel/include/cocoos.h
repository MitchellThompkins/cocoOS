#ifndef COCOOS_H
#define COCOOS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
