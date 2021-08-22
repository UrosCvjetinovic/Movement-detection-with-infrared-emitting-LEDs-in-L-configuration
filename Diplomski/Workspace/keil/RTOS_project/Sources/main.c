#include "Application.h"
#include "UserFeedback.h"
#include "ConfigureHardware.h"
#include "Priorities.h"
#include "Common.h"

int main_app(void);

int main(void)
{
	
  //Set the system clock to 80Mhz
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  
	ConfigureLEDS();
	
	// Startup Led feedback
	LedUserFeedbackStartup();
	
	ConfigureUART();
	EnableUART();
	LedUserFeedbackProgress();
	UARTwrite("UART is configured \n", 23);
	
	StartupTerminalOutput();
	
	ConfigureI2C();
	EnableI2C();
	LedUserFeedbackProgress();
	UARTwrite("I2C is configured \n", 22);
	
	main_app();
	//The following line should never be reached. Failure to allocate enough
	// memory from the heap would be one reason
	/* Infinite loop */
  while (1);
	
}
