#include "ConfigureHardware.h"

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
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

		
    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 9600, 16000000);

    // Set trigger for TX and RX interrupt for one sample
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
	
}
void EnableUART(void)
{
    // Enable the UART interrupt.
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX);
}
void ConfigureI2C(void)
{
    //
    // Enable the GPIO Peripheral used by the I2c.
    // 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    //
    // Enable I2C
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	
    // Configure the pins for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
	
	   // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.  For this example we will use a data rate of 100kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
		
    // Enable the I2C0 master module.
    I2CMasterEnable(I2C0_BASE);
}

void EnableI2C(void)
{
		I2CSlaveIntEnableEx(I2C0_BASE, I2C_SLAVE_INT_DATA);
    // Enable the I2C0 slave module.
    I2CSlaveEnable(I2C0_BASE);

    // Set the slave address to SLAVE_ADDRESS.
    I2CSlaveInit(I2C0_BASE, SLAVE_ADDRESS);

    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDRESS, false);

    // Initiate send of single piece of data from the master.  Since the
    // loopback mode is enabled, the Master and Slave units are connected
    // allowing us to receive the same data that we sent out.
    //
	
}

