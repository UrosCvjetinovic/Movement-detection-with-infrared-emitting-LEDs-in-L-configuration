#include "Application.h"
#include "ConfigHardware.h"
//#include <FreeRTOS.h>
//#include <task.h>
//#include "semphr.h"

#define SLAVE_ADDRESS 0x3C

uint8_t ui8Message;
extern uint8_t running;
//extern xSemaphoreHandle read_semaphore;
//xQueueHandle g_pQueue_SensorRead;
//xQueueHandle g_pQueue_R;
uint8_t running = 0;
//xQueueHandle g_pQueue_display;
//xSemaphoreHandle read_semaphore;
	
int main(void)
{
	
  //Set the system clock to 80Mhz
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  
	ConfigureLEDS();
	ConfigureUART();
	ConfigureI2C();
	SetLED(0);
	EnableUART();
	EnableI2C();
	
  while (1) {
    ui8Message = UARTgetc();
    UARTwrite(&ui8Message, 1);
  }
}


void UART0IntHandler(void)
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
            //xQueueSend(g_pQueue_R, &ui8Message, portMAX_DELAY);
            UARTCharGetNonBlocking(UART0_BASE);
        }
    }
}
