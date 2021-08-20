#include "Application.h"
/**
 * main.c
 */
#define GPIO_O_DATA 0x00000000 // GPIO Data
#define GPIO_O_DIR 0x00000400 // GPIO Direction

#define GPIO_PIN_0 0x00000001 // GPIO pin 0
#define GPIO_PIN_1 0x00000002 // GPIO pin 1
#define GPIO_PIN_2 0x00000004 // GPIO pin 2
#define GPIO_PIN_3 0x00000008 // GPIO pin 3
#define GPIO_PIN_4 0x00000010 // GPIO pin 4
#define GPIO_PIN_5 0x00000020 // GPIO pin 5
#define GPIO_PIN_6 0x00000040 // GPIO pin 6
#define GPIO_PIN_7 0x00000080 // GPIO pin 7

#define LED_PERIPH SYSCTL_PERIPH_GPIOC
void Wait() {
  SysCtlDelay(500 * (SysCtlClockGet() /3 / 1000));
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
  SysCtlPeripheralEnable(LED_PERIPH);
  SysCtlDelay(3);

	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

  while (1) {
		
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, GPIO_PIN_4);
    Wait();                                  // 500ms pause
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, GPIO_PIN_5);
    Wait();                                  // 500ms pause
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, 0);
    Wait();    
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0);
    Wait();                                // 500ms pause
  }
}
