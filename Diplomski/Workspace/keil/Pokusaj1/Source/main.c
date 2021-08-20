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


	uint8_t ui8Message;
	
void Wait() {
  SysCtlDelay(500 * (SysCtlClockGet() /3 / 1000));
}

void ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 9600, 16000000);
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
	ConfigureUART();
	
  while (1) {
    ui8Message = UARTgetc();
    UARTwrite(&ui8Message, 1);
  }
}
