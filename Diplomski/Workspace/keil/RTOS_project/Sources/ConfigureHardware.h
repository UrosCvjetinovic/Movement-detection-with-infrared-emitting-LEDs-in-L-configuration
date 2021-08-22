#ifndef CONFIGHARDWARE_H
#define CONFIGHARDWARE_H

#include "Application.h"

void ConfigureUART(void);
void EnableUART(void);
void ConfigureI2C(void);
void EnableI2C(void);
void ConfigureLEDS(void);
void toggleYellowLED(void);
void toggleRedLED(void);
void SetLED(uint8_t selectedYellowLed, uint8_t selectedRedLed);
void LedUserFeedback(void);
void Wait(uint16_t timems);
#endif //CONFIGHARDWARE_H
