#include "cmsis_os2.h" 

#include "Application.h"
#include "UserFeedback.h"
#include "Priorities.h"
#include "Common.h"

// Message Queue creation & usage
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
osMessageQueueId_t mid_MsgQueue_UART_I2C;                // message queue id
osMessageQueueId_t mid_MsgQueue_I2C_UART;                // message queue id
osMessageQueueId_t mid_MsgQueue_INT_UART;                // message queue id
 
// thread id
osThreadId_t tid_Thread_UART_TX;
osThreadId_t tid_Thread_UART_RX;
osThreadId_t tid_Thread_I2C_TX;
osThreadId_t tid_Thread_I2C_RX;


void Thread_UART_RX(void *pvParameters)
{ 
	uint8_t ui8Message;
  osStatus_t status;
	
	while(1) {
					toggleYellowLED();
					UARTwrite("Entered UART RX \n", 19);
    status = osMessageQueueGet(mid_MsgQueue_INT_UART, &ui8Message, NULL, osWaitForever);   // wait for message
					UARTwrite("Passed UART RX block \n", 25);
    if (status == osOK) {
					UARTwrite("Passed2 UART RX block \n", 25);
			osMessageQueuePut(mid_MsgQueue_UART_I2C, &ui8Message, 0U, osWaitForever);
    }
		osThreadYield();
	}
}
void Thread_UART_TX(void *pvParameters)
{ 
	uint8_t ui8Message;
  osStatus_t status;
	
	while(1) {
					toggleRedLED();
					UARTwrite("Entered UART TX \n", 19);
    status = osMessageQueueGet(mid_MsgQueue_I2C_UART, &ui8Message, NULL, osWaitForever);   // wait for message
					UARTwrite("Passed UART TX block \n", 25);
    if (status == osOK) {
					UARTwrite("Passed2 UART TX block \n", 25);
			UARTwrite("FromThreads: ", 14);
			UARTwrite(&ui8Message, 1);
					UARTwrite("\n", 3);
    }
		osThreadYield();
	}
}
void Thread_I2C_TX(void *pvParameters)
{
	uint8_t ui8Message;
  osStatus_t status;
	
	while (1) {
					toggleYellowLED();
					UARTwrite("Entered I2C TX \n", 18);
    status = osMessageQueueGet(mid_MsgQueue_UART_I2C, &ui8Message, NULL, osWaitForever);   // wait for message
					UARTwrite("Passed I2C TX block \n", 25);
    if (status == osOK) {
						UARTwrite("Passed2 I2C TX block \n", 25);
			I2CMasterDataPut(I2C0_BASE, ui8Message);
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
			osThreadFlagsSet(tid_Thread_I2C_RX, 0x1U);
    }
		osThreadYield();
	}
}
void Thread_I2C_RX(void *pvParameters)
{
	uint8_t ui8Message;
	
	while (1) {
				toggleRedLED();
				UARTwrite("Entered I2C RX \n", 18);
		osThreadFlagsWait(0x1U, osFlagsWaitAny, osWaitForever);
				UARTwrite("Passed I2C RX block \n", 25);
		ui8Message = I2CSlaveDataGet(I2C0_BASE);
		osMessageQueuePut(mid_MsgQueue_I2C_UART, &ui8Message, 0U, osWaitForever);
		osThreadYield();
	}
}

int main_app(void) 
{
  osStatus_t status;
	
  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize(); 
  
  /* Create new thread */
  tid_Thread_UART_TX = osThreadNew(Thread_UART_TX, NULL, NULL);
  if (tid_Thread_UART_TX == NULL) {
    return(-1);
  }
  tid_Thread_UART_RX = osThreadNew(Thread_UART_RX, NULL, NULL);
  if (tid_Thread_UART_RX == NULL) {
    return(-1);
  }
  tid_Thread_I2C_TX = osThreadNew(Thread_I2C_TX, NULL, NULL);
  if (tid_Thread_I2C_TX == NULL) {
    return(-1);
  }
  tid_Thread_I2C_RX = osThreadNew(Thread_I2C_RX, NULL, NULL);
  if (tid_Thread_I2C_RX == NULL) {
    return(-1);
  }  
	// Set priority
	status = osThreadSetPriority(tid_Thread_UART_TX, PRIORITY_THREAD_UART_TX);
  if (status != osOK) {
    return(-1);
  }
	status = osThreadSetPriority(tid_Thread_UART_RX, PRIORITY_THREAD_UART_RX);
  if (status != osOK) {
    return(-1);
  }
	status = osThreadSetPriority(tid_Thread_I2C_TX, PRIORITY_THREAD_I2C_TX);
  if (status != osOK) {
    return(-1);
  }
	status = osThreadSetPriority(tid_Thread_I2C_RX, PRIORITY_THREAD_I2C_RX);
  if (status != osOK) {
    return(-1);
  }
	LedUserFeedbackProgress();
	UARTwrite("Threads created\n", 17);
	
  mid_MsgQueue_INT_UART = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(uint8_t), NULL);
  if (mid_MsgQueue_INT_UART == NULL) {
    return(-1);
  }
  mid_MsgQueue_I2C_UART = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(uint8_t), NULL);
  if (mid_MsgQueue_I2C_UART == NULL) {
    return(-1);
  }
  mid_MsgQueue_UART_I2C = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(uint8_t), NULL);
  if (mid_MsgQueue_UART_I2C == NULL) {
    return(-1);
  }
	
	LedUserFeedbackProgress();
	UARTwrite("RTOS objects created\n", 23);
	
	
  /* Start thread execution */ 
  osKernelStart();
	  
}

void UART0_Handler(void)
{
	uint32_t ui32Status;
	uint8_t ui8Message;

	ui32Status = UARTIntStatus(UART0_BASE, true);

	UARTIntClear(UART0_BASE, ui32Status);

	// Receive
	if (ui32Status & UART_INT_RX) {
		if (UARTCharsAvail(UART0_BASE)) {
			UARTwrite("FromInterrupt: ", 14);
			ui8Message = UARTCharGet(UART0_BASE);
			UARTwrite(&ui8Message, 1);
			ui8Message = UARTCharGet(UART0_BASE);
			UARTwrite(&ui8Message, 1);
			UARTwrite("\n", 3);
			osMessageQueuePut(mid_MsgQueue_INT_UART, &ui8Message, 0U, 0U);
			osThreadResume(tid_Thread_UART_RX);
			UARTCharGetNonBlocking(UART0_BASE);
		}
	}
}
