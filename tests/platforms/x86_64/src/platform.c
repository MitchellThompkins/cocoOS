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
static int interval_ms = 3000;

static void *tick(void *interval_ms)
{
    if(timer_enable)
    {
        int interval_us = (*(int *)interval_ms)*1000;

        for(;;)
        {
            printf("ticking system time linux: %llu\n", get_system_time());
            tick_system_time();
            usleep(interval_us);
        }
    }
}

void platform_setup_timer(void)
{
    pthread_create(&thread, NULL, tick, &interval_ms);
}

void platform_enable_timer(void)
{
    timer_enable = true;
}

#ifdef __cplusplus
}
#endif
