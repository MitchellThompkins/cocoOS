#include "error.h"
#include "platform.h"

#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

static pthread_t thread;
static bool timer_enable = false;
static bool thread_enabled = false; // Stop us from re-enabling

static int32_t max_ticks = -1;

static void *tick(void *interval_ms)
{
    if(!thread_enabled && timer_enable)
    {
        thread_enabled = true;
        const uint32_t interval_us = (*(int*)interval_ms)*1000;

        for(;;)
        {
            tick_system_time();

            if( max_ticks != -1
                && get_system_time() >= max_ticks )
            {
                printf("ERR: max_ticks exceeded in a test!\n");
                unexpected_exit();
            }

            usleep(interval_us);
        }
    }
}

void set_tick_limit_before_exit(const int32_t limit)
{
    max_ticks = limit;
}

error_code_t platform_setup_timer(const uint32_t interval_ms)
{
    // otherwise compiler complains about discarding const with a ptr to a
    // uint16_t
    static uint32_t interval_ms_copy;
    interval_ms_copy = interval_ms;

    int status = pthread_create(&thread, NULL, tick, &interval_ms_copy);
    return (status == (int)NO_ERR) ? NO_ERR : CONFIG_ERR;
}

void platform_enable_timer(void)
{
    timer_enable = true;
}

void unexpected_exit(void)
{
    exit( (int)RUNTIME_ERR );
}

void expected_exit(void)
{
    exit( (int)NO_ERR );
}

#ifdef __cplusplus
}
#endif
