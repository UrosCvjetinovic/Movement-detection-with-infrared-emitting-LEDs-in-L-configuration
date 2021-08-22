#include "UserFeedback.h"

void Wait(uint16_t timems) {
  SysCtlDelay(timems * (SysCtlClockGet() /3 / 1000));
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
void StartupTerminalOutput()
{
	UARTwrite("********************************************\n", 46);
	UARTwrite("Program: Swipe detector\n", 26);
	UARTwrite("Author: Uros Cvjetinovic\n", 27);
	UARTwrite("Hardware: TM4C123GH6PM and ProximityClick13\n", 47);
	UARTwrite("********************************************\n", 46);
}

void LedUserFeedbackStartup()
{
	SetLED(0,0);
  Wait(100);
	SetLED(1,1);
  Wait(100);
	SetLED(0,0);
  Wait(100);
	SetLED(1,1);
  Wait(100);
	SetLED(0,0);
}
void LedUserFeedbackProgress()
{
	SetLED(0,0);
  Wait(100);
	SetLED(0,1);
  Wait(100);
	SetLED(1,0);
  Wait(100);
	SetLED(1,1);
  Wait(100);
	SetLED(0,0);
}