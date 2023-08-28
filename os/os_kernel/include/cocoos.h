#ifndef COCOOS_H
#define COCOOS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// This is a clever usage of Duff's Device which allows for nearly-re-entrant
// coroutines to be implemented in a fashion similar to that described in
// https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html. The nearly
// qualifier is because locally declared non-static variables will not persist
// between function calls, so users either need to declare their variables
// static or pass variables which are to be preserved between function calls to
// the task in the form of a void* when calling task_create().

#define OS_BEGIN    switch ( task_internal_state_get( os_get_running_tid() ) ) { case 0:

#define OS_END      os_task_kill( os_get_running_tid() );\
                    os_free_tid();\
                    return; }

#define OS_RELEASE  os_task_internal_state_set( os_get_running_tid(), __LINE__ );\
                    os_free_tid();\
                    return;\
                    case (__LINE__):

#define OS_WAIT_TICKS(time,id)  do { os_task_wait_time_set( os_get_running_tid(), id, time );\
                                OS_RELEASE; } while (0)

#ifdef __cplusplus
}
#endif

#endif
