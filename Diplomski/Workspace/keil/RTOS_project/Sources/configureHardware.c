#include "Application.h"

void Wait(uint16_t timems) {
  SysCtlDelay(timems * (SysCtlClockGet() /3 / 1000));
}

void ConfigureLEDS(void)
{
  /*
    Enables the clock on the GPIO, or "turns on"
    Also, delays for a bit since it's advised in the datasheet, i add problems
    when i didn't have that delay.
  */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
  SysCtlDelay(3);

	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
}

void toggleYellowLED()
{
	uint8_t state;
	state = GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_4);
	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, (~state & GPIO_PIN_4));
}
void toggleRedLED()
{
	uint8_t state;
	state = GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_5);
	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, (~state & GPIO_PIN_5));
}
void SetLED(uint8_t selectedYellowLed, uint8_t selectedRedLed)
{
	if(selectedYellowLed)
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, GPIO_PIN_4);
	else 
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, 0);
	if(selectedRedLed)
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, GPIO_PIN_5);
	else
		GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0);
}

void ConfigureUART(void)
{
	
    //
    // Enable the GPIO Peripheral used by the UART.
    // 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure GPIO Pins for UART mode.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    //UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

		
    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 9600, 16000000);

    // Set trigger for TX and RX interrupt for one sample
    //UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
	
}
void EnableUART(void)
{
    // Enable the UART interrupt.
    //IntEnable(INT_UART0);
    //UARTIntEnable(UART0_BASE, UART_INT_RX);
}
void ConfigureI2C(void)
{
    // Configure the pins for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
}

void EnableI2C(void)
{
    // Enable and initialize the I2C0 master module. (100kbps)
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    // Enable the I2C0 master module.
    I2CMasterEnable(I2C0_BASE);
}

