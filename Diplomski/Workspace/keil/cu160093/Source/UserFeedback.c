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


void write_hex(uint32_t data)
{
    uint8_t ui8temp;

    ui8temp = '0';
    UARTwrite(&ui8temp, 1);

    ui8temp = 'x';
    UARTwrite(&ui8temp, 1);

    if (data & 0xFFFFFF00) {

        if (data & 0xFFFF0000) {

            if (data & 0xFF000000) {

                ui8temp = ((data & 0xF0000000) >> 32) + 48;
                if (ui8temp > 57)
                    ui8temp += 7;
                UARTwrite(&ui8temp, 1);

                ui8temp = ((data & 0x0F000000) >> 28) + 48;
                if (ui8temp > 57)
                    ui8temp += 7;
                UARTwrite(&ui8temp, 1);

            }

            ui8temp = ((data & 0xF00000) >> 24) + 48;
            if (ui8temp > 57)
                ui8temp += 7;
            UARTwrite(&ui8temp, 1);

            ui8temp = ((data & 0x0F0000) >> 16) + 48;
            if (ui8temp > 57)
                ui8temp += 7;
            UARTwrite(&ui8temp, 1);
        }

        ui8temp = ((data & 0xF000) >> 12) + 48;
        if (ui8temp > 57)
            ui8temp += 7;
        UARTwrite(&ui8temp, 1);

        ui8temp = ((data & 0x0F00) >> 8) + 48;
        if (ui8temp > 57)
            ui8temp += 7;
        UARTwrite(&ui8temp, 1);
    }

    ui8temp = ((data & 0xF0) >> 4) + 48;
    if (ui8temp > 57)
        ui8temp += 7;
    UARTwrite(&ui8temp, 1);

    ui8temp = (data & 0x0F) + 48;
    if (ui8temp > 57)
        ui8temp += 7;
    UARTwrite(&ui8temp, 1);
}
