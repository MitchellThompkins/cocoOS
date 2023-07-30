#ifndef SYSTEM_TIME_H
#define SYSTEM_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void tick_system_time(void);
uint64_t get_system_time(void);

#ifdef __cplusplus
}
#endif

#endif
