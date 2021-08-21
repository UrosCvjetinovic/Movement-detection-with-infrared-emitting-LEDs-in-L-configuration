#ifndef CONFIGHARDWARE_H
#define CONFIGHARDWARE_H

#include "Application.h"

void ConfigureUART(void);
void EnableUART(void);
void ConfigureI2C(void);
void EnableI2C(void);
void ConfigureLEDS(void);
void SetLED(uint8_t selectedYellowLed, uint8_t selectedRedLed);
void toggleYellowLED();
void toggleRedLED();
void Wait();
#endif //CONFIGHARDWARE_H
