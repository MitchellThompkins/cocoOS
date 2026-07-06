#ifndef ANGEL_H
#define ANGEL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//https://github.com/ARM-software/abi-aa/blob/main/semihosting/semihosting.rst#semihosting-operations

typedef enum
{
    SYS_CLOCK = 0x10,
    SYS_CLOSE = 0x02,
    SYS_ELAPSED = 0x30,
    SYS_ERRNO = 0x13,
    SYS_EXIT = 0x18,
    SYS_EXIT_EXTENDED = 0x20,
    SYS_FLEN = 0x0c,
    SYS_GET_CMDLINE = 0x15,
    SYS_HEAPINFO = 0x16,
    SYS_ISERROR = 0x08,
    SYS_ISTTY = 0x09,
    SYS_OPEN = 0x01,
    SYS_READ = 0x06,
    SYS_READC = 0x07,
    SYS_REMOVE = 0x0e,
    SYS_RENAME = 0x0f,
    SYS_SEEK = 0x0a,
    SYS_SYSTEM = 0x12,
    SYS_TICKFREQ = 0x31,
    SYS_TIME = 0x11,
    SYS_TMPNAM = 0x0d,
    SYS_WRITE = 0x05,
    SYS_WRITEC = 0x03,
    SYS_WRITE0 = 0x04,
} angel_swi_t;

typedef enum
{
    Stopped_BranchThroughZer0 = 0x20000,
    Stopped_UndefinedInstr = 0x20001,
    Stopped_SoftwareInterrupt = 0x20002,
    Stopped_PrefetchAbort = 0x20003,
    Stopped_DataAbort  = 0x20004,
    Stopped_AddressException = 0x20005,
    Stopped_IRQ   = 0x20006,
    Stopped_FIQ   = 0x20007,
    Stopped_BreakPoint  = 0x20020,
    Stopped_WatchPoint  = 0x20021,
    Stopped_StepComplete = 0x20022,
    Stopped_RunTimeErrorUnknown = 0x20023,
    Stopped_InternalError = 0x20024,
    Stopped_UserInterruption = 0x20025,
    Stopped_ApplicationExit = 0x20026,
    Stopped_StackOverflow = 0x20027,
    Stopped_DivisionByZero = 0x20028,
    Stopped_OSSpecific  = 0x20029,

} angel_swi_report_t;

void angel_swi_call(uint32_t type, void *param);

void angel_exit(const int status);

#ifdef __cplusplus
}
#endif

#endif
