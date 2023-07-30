#include "angel.h"
#include "cpu_a9.h"

#ifdef __cplusplus
extern "C" {
#endif

void angel_exit(const int status)
{
    if(status == 0)
    {
        angel_swi_call( SYS_EXIT,
                (void *) Stopped_ApplicationExit);
    }
    else
    {
        angel_swi_call( SYS_EXIT_EXTENDED,
                (void *) Stopped_ApplicationExit);
    }
}

#ifdef __cplusplus
}
#endif
