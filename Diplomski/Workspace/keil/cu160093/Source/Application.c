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

osTimerId_t periodic_id;

void Thread_Gesture(void *pvParameters)
{ 
    uint16_t ui16Output [3];
    uint8_t ui8Diode = 0;
    uint8_t arrayIndex = 0;
    uint16_t rgi16IR1[5];
    uint16_t rgi16IR2[5];
    uint16_t rgi16IR3[5];
		osStatus_t status;
	
		status = osThreadSetPriority(tid_Thread_Gesture, PRIORITY_GESTURE_THREAD);
		if (status != osOK) {
			UARTwrite("Thread priority not set\n", 25);
		}
		
    while (1) {
				status = osMessageQueueGet(mid_MsgQueue_SensorRead, ui16Output, NULL, osWaitForever);   // wait for message
				if (status == osOK) {
						rgi16IR1[arrayIndex] = ui16Output[0];
						rgi16IR2[arrayIndex] = ui16Output[1];
						rgi16IR3[arrayIndex++] = ui16Output[2];
					}
			if (arrayIndex > 4)
					arrayIndex = 0;
			osThreadYield();

			}
}
void Thread_Read(void *pvParameters)
{ 
    uint16_t ui16Output = 0;
		uint16_t auiDiodeValue [3];
		osStatus_t status;

		status = osThreadSetPriority(tid_Thread_Read, PRIORITY_READ_THREAD);
		if (status != osOK) {
			UARTwrite("Thread priority not set\n", 25);
		}
		
    while (1) {
				osThreadFlagsWait(0x1U, osFlagsWaitAny, osWaitForever);
				StartMessuring();
				Wait(50);
			
        ui16Output = ReadOutput(0);
				auiDiodeValue[0] = ui16Output;
						UARTwrite("IR1 diode = ", 25);
						write_hex(ui16Output);
						UARTwrite("\n", 3);
			
        ui16Output = ReadOutput(2);
				auiDiodeValue[1] = ui16Output;
						UARTwrite("IR2 diode = ", 25);
						write_hex(ui16Output);
						UARTwrite("\n", 3);
			
        ui16Output = ReadOutput(4);
				auiDiodeValue[2] = ui16Output;
						UARTwrite("IR3 diode = ", 25);
						write_hex(ui16Output);
						UARTwrite("\n", 3);
				
				osMessageQueuePut(mid_MsgQueue_SensorRead, auiDiodeValue, 0U, osWaitForever);
        //if (ui8OutputAddr  )

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
        UARTwrite("\n\nCommands ( p - sensor info; d - setup daytime; n - setup nighttime; s - start; c - stop )", 95);
        UARTwrite("\nEnter command: ", 16);
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
								case 'd':
										UARTwrite("\nSETING UP SENSOR...",20);
										Setup(0x07);
										UARTwrite("\n...SENSOR READY",20);
										break;
								case 'n':
										UARTwrite("\nSETING UP SENSOR...",20);
										Setup(0x0A);
										UARTwrite("\n...SENSOR READY",20);
										break;
								case 's':
										UARTwrite("\nPROGRAM STARTED... ",20);
										status = osTimerStart(periodic_id, 520U);
										if (status != osOK) {
											// Timer could not be stopped
										}
										break;
								case 'c':
										UARTwrite("\n...PROGRAM STOPED",20);
										osTimerDelete(periodic_id);
										status = osTimerStop(periodic_id);                     // stop timer
										if (status != osOK) {
											// Timer could not be stopped
										}
										break;
								default:
										UARTwrite("\nNo command registered",22);
						}
					}
			osThreadYield();
    }
}

static void periodic_Callback (void *argument) {
  //int32_t arg = (int32_t)argument; // cast back argument '5'
  // do something, i.e. set thread/event flags
	osThreadFlagsSet(tid_Thread_Read, 0x1U);
}

int main_app(void) 
{
  osStatus_t status;
	
	// creates a periodic timer:
	periodic_id = osTimerNew(periodic_Callback, osTimerPeriodic, (void *)5, NULL); // (void*)5 is passed as an argument
                                                                               // to the callback function
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
	
  mid_MsgQueue_SensorRead = osMessageQueueNew(MSGQUEUE_OBJECTS, 3*sizeof(uint16_t), NULL);
  if (mid_MsgQueue_SensorRead == NULL) {
    return(-1);
  }
	mid_MsgQueue_UserInput = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(uint8_t), NULL);
  if (mid_MsgQueue_UserInput == NULL) {
    return(-1);
  }
	LedUserFeedbackProgress();
	UARTwrite("CMSIS RTOS2 objects initialized\n", 32);
	
	
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
			//osThreadResume(Thread_Communication);
			UARTCharGetNonBlocking(UART0_BASE);
		}
	}
}
