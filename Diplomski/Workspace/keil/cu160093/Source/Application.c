#include "cmsis_os2.h" 

#include "Application.h"
#include "UserFeedback.h"
#include "Priorities.h"
#include "Common.h"
#include "ProximityClickApi.h"

// Message Queue creation & usage
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
osMessageQueueId_t mid_MsgQueue_SensorRead;                // message queue id
osMessageQueueId_t mid_MsgQueue_UserInput;                // message queue id
 
// thread id
osThreadId_t tid_Thread_Communication;
osThreadId_t tid_Thread_Read;
osThreadId_t tid_Thread_Gesture;

uint8_t running;

void Thread_Gesture(void *pvParameters)
{ 
	
    uint16_t ui16Output = 0;
    uint8_t ui8Diode = 0;
    uint8_t arrayIndex = 0;
    uint16_t rgi16SmallIR[5];
    uint16_t rgi16MediumIR[5];
    uint16_t rgi16LargeIR[5];
		osStatus_t status;
	
		status = osThreadSetPriority(tid_Thread_Gesture, PRIORITY_GESTURE_THREAD);
		if (status != osOK) {
			UARTwrite("Thread priority not set\n", 25);
		}
		
    while (1) {
        if (running) {
						UARTwrite("Thread Gesture\n", 25);
						status = osMessageQueueGet(mid_MsgQueue_SensorRead, &ui16Output, NULL, osWaitForever);   // wait for message
									UARTwrite("Passed UART RX block \n", 25);
						UARTwrite("Thread Gesture\n", 25);
						if (status == osOK) {
							switch (ui8Diode) {
									case 0:
											rgi16SmallIR[arrayIndex++] = ui16Output;
											ui8Diode++;
											break;
									case 1:
											rgi16MediumIR[arrayIndex++] = ui16Output;
											ui8Diode++;
											break;
									case 2:
											rgi16LargeIR[arrayIndex++] = ui16Output;
											ui8Diode++;
											break;
									default:
											ui8Diode = 0;
							}
							if (arrayIndex > 4)
									arrayIndex = 0;

					}
			}
		osThreadYield();
	}
}
void Thread_Read(void *pvParameters)
{ 
    uint8_t ui8OutputAddr = 0;
    uint8_t ui16Output = 0;
		osStatus_t status;

		status = osThreadSetPriority(tid_Thread_Read, PRIORITY_READ_THREAD);
		if (status != osOK) {
			UARTwrite("Thread priority not set\n", 25);
		}
		
    while (1) {
				UARTwrite("Thread Read\n", 25);
				osThreadFlagsWait(0x1U, osFlagsWaitAny, osWaitForever);
				UARTwrite("Thread Read\n", 25);

        ui16Output = ReadOutput(ui8OutputAddr++);
				osMessageQueuePut(mid_MsgQueue_SensorRead, &ui16Output, 0U, osWaitForever);
        if (ui8OutputAddr  )

        if (running)
						osThreadFlagsSet(tid_Thread_Read, 0x1U);
			osThreadYield();	

    }
}
void Thread_Communication(void *pvParameters)
{
	
    uint8_t ui8Message;
    uint32_t ui32Request;
		osStatus_t status;
	
		status = osThreadSetPriority(tid_Thread_Communication, PRIORITY_COM_THREAD);
		if (status != osOK) {
			UARTwrite("Thread priority not set\n", 25);
		}
    while (1) {
				UARTwrite("Comm\n", 25);
        UARTwrite("\n\nCommands ( p - sensor info; a - arm/setup; s - start; c - stop )", 60);
        UARTwrite("\nEnter command: ", 14);
				status = osMessageQueueGet(mid_MsgQueue_UserInput, &ui8Message, NULL, osWaitForever);   // wait for message
				if (status == osOK) {
						switch (ui8Message) {
								case 'p':
										ui32Request = GetSpecs();
										UARTwrite("\nSensor information:",20);
										UARTwrite("\nPART ID is: ",20);
										write_hex((ui32Request & 0xFF0000) >> 16);
										UARTwrite("\nHARDWARE ID is: ",20);
										write_hex((ui32Request & 0xFF00) >> 8);
										UARTwrite("\nREV ID is: ",20);
										write_hex(ui32Request & 0xFF);
										break;
								case 'a':
										UARTwrite("\nSETING UP SENSOR...",20);
										Setup();
										UARTwrite("\n...SENSOR READY",20);
										break;
								case 's':
										UARTwrite("\nPROGRAM STARTED... ",20);
										Start();
										running = 1;
										osThreadFlagsSet(tid_Thread_Read, 0x1U);
										break;
								case 'c':
										UARTwrite("\n...PROGRAM STOPED",20);
										running = 0;
										break;
								default:
										UARTwrite("\nNo command registered",20);
						}
					}
			osThreadYield();
    }
}

int main_app(void) 
{
  osStatus_t status;
	
  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize(); 
  
  /* Create new thread */
  tid_Thread_Communication = osThreadNew(Thread_Communication, NULL, NULL);
  if (tid_Thread_Communication == NULL) {
    return(-1);
  }
  tid_Thread_Read = osThreadNew(Thread_Read, NULL, NULL);
  if (tid_Thread_Read == NULL) {
    return(-1);
  }
  tid_Thread_Gesture = osThreadNew(Thread_Gesture, NULL, NULL);
  if (tid_Thread_Gesture == NULL) {
    return(-1);
  }
	LedUserFeedbackProgress();
	UARTwrite("Threads initialized\n", 22);
	
  mid_MsgQueue_SensorRead = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(uint16_t), NULL);
  if (mid_MsgQueue_SensorRead == NULL) {
    return(-1);
  }
	mid_MsgQueue_UserInput = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(uint8_t), NULL);
  if (mid_MsgQueue_UserInput == NULL) {
    return(-1);
  }
	LedUserFeedbackProgress();
	UARTwrite("CMSIS RTOS2 objects initialized\n", 27);
	
	
  /* Start thread execution */ 
  osKernelStart();
	  
	while(1);
		
	return 0;
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
			ui8Message = UARTCharGet(UART0_BASE);
			UARTwrite(&ui8Message, 1);
			osMessageQueuePut(mid_MsgQueue_UserInput, &ui8Message, 0U, 0U);
			osThreadResume(Thread_Communication);
			UARTCharGetNonBlocking(UART0_BASE);
		}
	}
}
