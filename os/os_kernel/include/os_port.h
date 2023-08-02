#ifndef OS_PORT_H_
#define OS_PORT_H_

//TODO(mthompkins): I think the user should be responsible for implementing
//these, so I plan to remove these.
//#include <interrupt.h>
//#define os_enable_interrupts()  __enable_irq()
//#define os_disable_interrupts() __disable_irq()

#define os_enable_interrupts()
#define os_disable_interrupts()

#endif
