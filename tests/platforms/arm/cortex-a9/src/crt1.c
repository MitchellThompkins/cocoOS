#include "stdbool.h"
#include "stdint.h"
#include "stddef.h"
#include "rdimon.h"
#include "angel.h"

#ifdef __cplusplus
extern "C" {
#endif

extern __attribute__((long_call)) int main(int argc, char *argv[]);

extern uint32_t __bss_start;
extern uint32_t __bss_end;

extern uint32_t __data_start;
extern uint32_t __data_end;
extern uint32_t __etext;

extern void (*__init_array_start[])(void);
extern void (*__init_array_end[])(void);

extern void (*__preinit_array_start[])(void);
extern void (*__preinit_array_end[])(void);

extern void (*__fini_array_start[])(void);
extern void (*__fini_array_end[])(void);


void _start(void);
void _fini(void);

static void ctors(void);
static void dtors(void);

void _fini()
{
    while (true)
    {}
}

__attribute__((constructor)) void semihost_init(void)
{
    initialise_monitor_handles();
}

void _start(void)
{
    // Copy data from ROM to RAM
    uint32_t *src = &__etext;
    uint32_t *dst = &__data_start;

    while(dst < &__data_end)
    {
        *dst = *src;
        dst++;
        src++;
    }

    // Clear the bss
    uint32_t *d = &__bss_start;
    while(d < &__bss_end)
    {
        *d = 0u;
        d++;
    }

    // Do constructor init
    ctors();

    // Jump to main()
    char **argv = NULL;
    char argc = 0;

    int status = main(argc, argv);
    dtors();

    angel_exit(status);
}

static void ctors(void)
{
    static bool initialized = false;

    if(!initialized)
    {
        uint32_t i;
        uint32_t count;

        count = (uint32_t)(__preinit_array_end - __preinit_array_start);
        for(i=0; i<count; i++)
        {
            __preinit_array_start[i]();
        }

        count = (uint32_t)(__init_array_end - __init_array_start);
        for(i=0; i<count; i++)
        {
            __init_array_start[i]();
        }

        initialized = true;
    }
}

// It actually seems like the global destructors aren't being called here, I'm
// guessing global destructors aren't getting placed in the .fini_array region
// like constructors do for .init_array but I'm not sure why
static void dtors(void)
{
    uint32_t i;
    uint32_t count;

    count = (uint32_t)(__fini_array_end - __fini_array_start);
    for(i=0; i<count; i++)
    {
        __fini_array_start[i]();
    }
}


// https://lists.debian.org/debian-gcc/2003/07/msg00057.html
void* __dso_handle = (void*) &__dso_handle;

#ifdef __cplusplus
}
#endif
