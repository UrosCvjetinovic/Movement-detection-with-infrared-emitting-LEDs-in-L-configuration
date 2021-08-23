#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"
#include "inc/hw_types.h"
#include "driverlib/fpu.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/i2c.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "common.h"
#include "read_task.h"
#include "communication_task.h"

#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line) {}
#endif

void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    while(1);
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
            xQueueSend(g_pQueue_R, &ui8Message, portMAX_DELAY);
            UARTCharGetNonBlocking(UART0_BASE);
        }
    }
}

void ConfigureHardware(void)
{

    ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                       SYSCTL_OSC_MAIN);

    // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    // Enable UART0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Enable the GPIO Peripheral used by the I2C
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // Enable I2C
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

}

void ConfigureUART(void)
{
    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);

    // Set trigger for TX and RX interrupt for one sample
    UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);

    // Enable the UART interrupt.
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX);
}

void ConfigureI2C(void)
{
    // Configure the pins for I2C0 functions on port B2 and B3.
    ROM_GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    ROM_GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.
    ROM_GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    ROM_GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Enable and initialize the I2C0 master module. (100kbps)
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    // Enable the I2C0 master module.
    I2CMasterEnable(I2C0_BASE);
}

int main(void)
{
    ConfigureHardware();
    ConfigureUART();
    ConfigureI2C();

    UARTprintf("\n\nWelcome to the FreeRTOS proximity setup!\n");

    if (ComTaskInit() != 0) {
        while(1);
    }

    if (ReadTaskInit() != 0) {
        while(1);
    }

    vTaskStartScheduler();

    while (1);
}
