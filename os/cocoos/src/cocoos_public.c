// Why do this monstrosity? Because not all users use cmake as a build system
// and this provides an easy way for them to include a single source file to
// specify with their build system

#include "../os_event/src/os_event.c"
#include "../os_kernel/src/os_cbk.c"
#include "../os_kernel/src/os_kernel.c"
#include "../os_msgqueue/src/os_msgqueue.c"
#include "../os_sem/src/os_sem.c"
#include "../os_task/src/os_task.c"
#include "../os_utils/src/os_assert.c"
