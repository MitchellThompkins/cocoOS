#include "os_applAPI.h"

/************************************************************** *******************/
/*  void os_cbkSleep( void )    *//**
*   Callback called by the os kernel when all tasks are in waiting state. Here you 
*   can put the MCU to low power mode. Remember to keep the clock running so we can 
*	wake up from sleep.
*
*		
*       
*/
/*********************************************************************************/
void os_cbkSleep( void ) {
    /* Enter low power mode here */
}
