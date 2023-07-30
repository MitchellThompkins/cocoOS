#include "error.h"
#include "platform.h"

#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

static pthread_t thread;
static bool timer_enable = false;

static void *tick(void *interval_ms)
{
    if(timer_enable)
    {
        int interval_us = (*(int *)interval_ms)*1000;

        for(;;)
        {
            tick_system_time();
            usleep(interval_us);
        }
    }
}

error_code_t platform_setup_timer(const uint16_t interval_ms)
{
    // otherwise compiler complains about discarding const with a ptr to a
    // uint16_t
    uint16_t interval_ms_copy = interval_ms;
    int status = pthread_create(&thread, NULL, tick, &interval_ms_copy);
    return (status == (int)NO_ERR) ? NO_ERR : CONFIG_ERR;
}

void platform_enable_timer(void)
{
    timer_enable = true;
}

#ifdef __cplusplus
}
#endif
