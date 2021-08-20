#include "Application.h"
#include <FreeRTOS.h>
#include <task.h>
#include "driverlib/i2c.h"
#include "semphr.h"
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
SemaphoreHandle_t g_pSemaphoreI2C = NULL;
SemaphoreHandle_t g_pSemaphoreUART = NULL;


void ConfigureHardware(void)
{

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    // Enable the GPIO Peripheral used by the UART.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Enable UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Enable the GPIO Peripheral used by the I2C
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Enable I2C
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

}

void ConfigureUART(void)
{
    // Configure GPIO Pins for UART mode.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 9600, 16000000);
}

void ConfigureI2C(void)
{
    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Enable loopback mode.
    I2CLoopbackEnable(I2C0_BASE);
    //HWREG(I2C0_BASE + I2C_O_MCR) |= 0x01;

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.  For this example we will use a data rate of 100kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

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

void vUartTaskRX(void *pvParameters)
{ 
	while(1) {
		ui8Message = UARTgetc();
		UARTwrite(&ui8Message, 1);
    xSemaphoreGive(g_pSemaphoreI2C);
	}
}
void vUartTaskTX(void *pvParameters)
{ 
		while(1) {
			xSemaphoreTake(g_pSemaphoreUART, portMAX_DELAY);
			UARTwrite(&ui8Message, 1);
			UARTwrite(&ui8MessageI2C, 1);
		}
}
void vI2CTaskTX(void *pvParameters)
{
    while (1) {
			I2CMasterDataPut(I2C0_BASE, ui8Message);
			I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
      xSemaphoreGive(g_pSemaphoreUART);
		}
}
void vI2CTaskRX(void *pvParameters)
{

    while (1) {
			xSemaphoreTake(g_pSemaphoreI2C, portMAX_DELAY);
			ui8MessageI2C = I2CSlaveDataGet(I2C0_BASE);
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
			
	ConfigureHardware();
	ConfigureUART();
	ConfigureI2C();
	
	if(xTaskCreate(vUartTaskTX, (const portCHAR *)"UARTTransmit",
                   RXUARTTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_I2C_TX_TASK, NULL) != pdTRUE)
	{
			while(1);
	}
	if(xTaskCreate(vUartTaskRX, (const portCHAR *)"UARTRecieve",
                   TXUARTTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_I2C_TX_TASK, NULL) != pdTRUE)
	{
			while(1);
	}
	if(xTaskCreate(vI2CTaskTX, (const portCHAR *)"I2CTransmit",
                   TXI2CTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_I2C_TX_TASK, NULL) != pdTRUE)
	{
			while(1);
	}
	if(xTaskCreate(vI2CTaskRX, (const portCHAR *)"I2CRecieve",
                   RXI2CTASKSTACKSIZE, NULL, tskIDLE_PRIORITY +
                   PRIORITY_I2C_TX_TASK, NULL) != pdTRUE)
	{
			while(1);
	}
	
	g_pSemaphoreI2C = xSemaphoreCreateMutex();
	g_pSemaphoreUART = xSemaphoreCreateMutex();
	
	//Startup of the FreeRTOS scheduler. The program should block here
	vTaskStartScheduler();
	
	//The following line should never be reached. Failure to allocate enough
	// memory from the heap would be one reason
  while (1);
}
