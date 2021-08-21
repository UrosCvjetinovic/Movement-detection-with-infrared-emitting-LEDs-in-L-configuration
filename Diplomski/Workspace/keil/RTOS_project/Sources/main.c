#include "cmsis_os2.h" 

#include "Application.h"
#include "configureHardware.h"

//#include "driverlib/i2c.h"
//#include "semphr.h"
#include "priorities.h"
#include "common.h"

//*****************************************************************************
//
// Set the address for slave module. This is a 7-bit address sent in the
// following format:
//                      [A6:A5:A4:A3:A2:A1:A0:RS]
//
// A zero in the "RS" position of the first byte means that the master
// transmits (sends) data to the selected slave, and a one in this position
// means that the master receives data from the slave.
//
//*****************************************************************************
#define SLAVE_ADDRESS 0x3C
uint8_t ui8Message, ui8MessageI2C;

// Semaphore id
osSemaphoreId_t g_pSemaphoreI2C;
osSemaphoreId_t g_pSemaphoreUART;
// thread id
osThreadId_t tid_vUartTaskTX;
osThreadId_t tid_vUartTaskRX;
osThreadId_t tid_vUartTaskTX;
osThreadId_t tid_vI2CTaskTX;
osThreadId_t tid_LedBlinky;

void vUartTaskRX(void *pvParameters)
{ 
	while(1) {
		toggleYellowLED();
		ui8Message = UARTgetc();
		UARTwrite(&ui8Message, 1);
		osSemaphoreRelease(g_pSemaphoreI2C);
    //xSemaphoreGive(g_pSemaphoreI2C);
	}
}
void vUartTaskTX(void *pvParameters)
{ 
		while(1) {
			toggleRedLED();
			osSemaphoreAcquire(g_pSemaphoreUART, osWaitForever);
			//xSemaphoreTake(g_pSemaphoreUART, portMAX_DELAY);
			UARTwrite(&ui8Message, 1);
			UARTwrite(&ui8MessageI2C, 1);
		}
}
void vI2CTaskTX(void *pvParameters)
{
    while (1) {
			toggleYellowLED();
			I2CMasterDataPut(I2C0_BASE, ui8Message);
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
			osSemaphoreRelease(g_pSemaphoreUART);
      //xSemaphoreGive(g_pSemaphoreUART);
		}
}
void vI2CTaskRX(void *pvParameters)
{

    while (1) {
			toggleRedLED();
			osSemaphoreAcquire(g_pSemaphoreI2C, osWaitForever);
			//xSemaphoreTake(g_pSemaphoreI2C, portMAX_DELAY);
			ui8MessageI2C = I2CSlaveDataGet(I2C0_BASE);
		}
}

void LedBlinky(void *pvParameters)
{
  for (;;) {
	  Wait(750);
		UARTwrite("L", 1);
		toggleRedLED();
		toggleYellowLED();
	  Wait(750);
  }
}

int main(void)
{
	
  //Set the system clock to 80Mhz
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  
	ConfigureLEDS();
	
	//DebugLeds
	SetLED(0,0);
  Wait(100);
	SetLED(1,1);
  Wait(100);
	SetLED(0,0);
  Wait(100);
	SetLED(1,1);
  Wait(100);
	SetLED(0,0);
	while(1)
	{
		SetLED(1,1);
		ui8Message = UARTgetc();
		if (ui8Message == 'o'){
			SetLED(0,0);
			Wait(300);
			SetLED(1,0);
			Wait(300);
			SetLED(1,1);
			Wait(300);
			SetLED(0,1);
			Wait(300);
			SetLED(0,0);
		}
	  Wait(750);
		SetLED(0,0);
	}
	
	UARTwrite("1", 2);
	SetLED(0,0);
  Wait(1000);
	UARTwrite("2", 2);
	SetLED(1,0);
  Wait(1000);
	UARTwrite("3", 2);
	SetLED(1,1);
  Wait(1000);
	UARTwrite("4", 2);
	SetLED(0,1);
  Wait(1000);
	UARTwrite("5", 2);
	SetLED(0,0);
	
	ConfigureI2C();
	EnableI2C();
	
	UARTwrite("1", 2);
	SetLED(0,0);
  Wait(1000);
	UARTwrite("2", 2);
	SetLED(1,0);
  Wait(1000);
	UARTwrite("3", 2);
	SetLED(1,1);
  Wait(1000);
	UARTwrite("4", 2);
	SetLED(0,1);
  Wait(1000);
	UARTwrite("5", 2);
	SetLED(0,0);
	
	
	
  /* Initialize CMSIS-RTOS */
  osKernelInitialize(); 
  
  /* Create new thread */
  osThreadNew(vUartTaskTX, NULL, NULL);
  osThreadNew(vUartTaskRX, NULL, NULL);
  osThreadNew(vI2CTaskTX, NULL, NULL);
  osThreadNew(vI2CTaskRX, NULL, NULL);
  osThreadNew(LedBlinky, NULL, NULL);

	//multiplex_id = osSemaphoreNew(3U, 3U, NULL);
  g_pSemaphoreI2C = osSemaphoreNew(3U, 3U, NULL);
  g_pSemaphoreUART = osSemaphoreNew(3U, 3U, NULL);
	
  /* Start thread execution */ 
  osKernelStart();
	  
	//The following line should never be reached. Failure to allocate enough
	// memory from the heap would be one reason
	/* Infinite loop */
  while (1)
  {
		SetLED(0,0);
    /* Add application code here */
    Wait(1000);
  }
	
  while (1);
}


void UART0_Handler(void)
{
    uint32_t ui32Status;
    uint8_t ui8Message;

    ui32Status = UARTIntStatus(UART0_BASE, true);

    UARTIntClear(UART0_BASE, ui32Status);

		UARTwrite("1", 2);
		SetLED(0,0);
		Wait(1000);
		UARTwrite("2", 2);
		SetLED(1,0);
		Wait(1000);
		UARTwrite("3", 2);
		SetLED(1,1);
		Wait(1000);
		UARTwrite("4", 2);
		SetLED(0,1);
		Wait(1000);
		UARTwrite("5", 2);
		SetLED(0,0);
	
    // Receive
    if (ui32Status & UART_INT_RX) {
        if (UARTCharsAvail(UART0_BASE)) {
            ui8Message = UARTCharGet(UART0_BASE);
            UARTwrite(&ui8Message, 1);
            //xQueueSend(g_pQueue_R, &ui8Message, portMAX_DELAY);
            UARTCharGetNonBlocking(UART0_BASE);
        }
    }
}

