#ifndef _os_defs
#define _os_defs

#include "user_os_config.h"

#ifndef N_TASKS
#error N_TASKS must be defined with #define
#endif
#ifdef __cplusplus
static_assert(N_TASKS>=0 && N_TASKS<255, "Maximum number of tasks is 254");
#else
_Static_assert(N_TASKS>=0 && N_TASKS<255, "Maximum number of tasks is 254");
#endif


#ifndef N_QUEUES
#error N_QUEUES must be defined with #define
#endif
#ifdef __cplusplus
static_assert(N_QUEUES >= 0 && N_QUEUES < 255, "Maximum number of queues is 254");
#else
_Static_assert(N_QUEUES >= 0 && N_QUEUES < 255, "Maximum number of queues is 254");
#endif


#ifndef N_SEMAPHORES
#error N_SEMAPHORES must be defined with #define
#endif
#ifdef __cplusplus
static_assert(N_SEMAPHORES >=0 && N_SEMAPHORES < 255, "Maximum number of queues is 254");
#else
_Static_assert(N_SEMAPHORES >=0 && N_SEMAPHORES < 255, "Maximum number of queues is 254");
#endif


#ifndef N_QUEUES
#error N_EVENTS must be defined with #define
#endif
#ifdef __cplusplus
static_assert(N_EVENTS >=0 && N_EVENTS < 255, "Maximum number of events is 254");
#else
_Static_assert(N_EVENTS >=0 && N_EVENTS < 255, "Maximum number of events is 254");
#endif


#ifndef N_QUEUES
#error ROUND_ROBIN must be defined as 0 or 1
#endif
#ifdef __cplusplus
static_assert(ROUND_ROBIN == 0 || ROUND_ROBIN == 1, "ROUND_ROBIN must be 0 or 1");
#else
_Static_assert(ROUND_ROBIN == 0 || ROUND_ROBIN == 1, "ROUND_ROBIN must be 0 or 1");
#endif


#ifndef Mem_t
#error A type (like uint32_t for example) must be defined for Mem_t
#endif

#define NO_MSG_ID   0xff
#define ISR_TID     0xfe

/* Total number of semaphores needed */
#define N_TOTAL_SEMAPHORES    ( N_SEMAPHORES + N_QUEUES )


/* Total number of events needed */
#define N_TOTAL_EVENTS        ( N_EVENTS + N_QUEUES )

#endif
