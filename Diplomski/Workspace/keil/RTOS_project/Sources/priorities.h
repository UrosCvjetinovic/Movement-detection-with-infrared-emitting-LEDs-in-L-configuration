#ifndef __PRIORITIES_H__
#define __PRIORITIES_H__

//*****************************************************************************
//
// The priorities of the various tasks.
//
//*****************************************************************************
#define PRIORITY_THREAD_UART_TX       osPriorityNormal
#define PRIORITY_THREAD_UART_RX       osPriorityLow
#define PRIORITY_THREAD_I2C_TX        osPriorityBelowNormal
#define PRIORITY_THREAD_I2C_RX        osPriorityAboveNormal


#endif // __PRIORITIES_H__
