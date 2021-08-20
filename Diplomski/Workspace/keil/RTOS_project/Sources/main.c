#include "Application.h"
#include <FreeRTOS.h>
#include <task.h>


void Wait() {
  SysCtlDelay(500 * (SysCtlClockGet() /3 / 1000));
}

void vPeriodicTask(void *pvParameters)
{
	//Establish the task's period
	const TickType_t xDelay = pdMS_TO_TICKS(1000);
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for(;;) {
		
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, GPIO_PIN_4);
    Wait();                                  // 500ms pause
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, GPIO_PIN_5);
    Wait();                                  // 500ms pause
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, 0);
    Wait();    
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0);
    Wait();    
		//Block until the next release time
		vTaskDelayUntil(&xLastWakeTime, xDelay);
	}
}
int main(void)
{
	
  //Set the system clock to 80Mhz
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  
  /*
    Enables the clock on the GPIO, or "turns on"
    Also, delays for a bit since it's advised in the datasheet, i add problems
    when i didn't have that delay.
  */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  SysCtlDelay(3);

	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	xTaskCreate(vPeriodicTask, "My Task", 256, NULL, 1, NULL);
	
	//Startup of the FreeRTOS scheduler. The program should block here
	vTaskStartScheduler();
	
	//The following line should never be reached. Failure to allocate enough
	// memory from the heap would be one reason
  while (1);
}
