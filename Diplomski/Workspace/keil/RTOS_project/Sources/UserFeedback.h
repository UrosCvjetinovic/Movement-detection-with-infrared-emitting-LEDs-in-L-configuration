#ifndef USERFEEDBACK_H
#define USERFEEDBACK_H

#include "Application.h"

void toggleYellowLED(void);
void toggleRedLED(void);
void SetLED(uint8_t selectedYellowLed, uint8_t selectedRedLed);
void LedUserFeedbackStartup(void);
void LedUserFeedbackProgress(void);
void StartupTerminalOutput(void);
void Wait(uint16_t timems);
#endif //USERFEEDBACK_H
