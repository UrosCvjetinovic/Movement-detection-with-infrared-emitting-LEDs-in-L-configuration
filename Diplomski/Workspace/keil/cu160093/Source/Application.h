#ifndef APPLICATION_H
#define APPLICATION_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


#define PART_TM4C123GH6PM
//#define TARGET_IS_TM4C123_
#define UART_BUFFERED
#define SLAVE_ADDRESS 0x3C

#include "driverlib\gpio.h"
#include "driverlib\interrupt.h"
#include "driverlib\pin_map.h"
#include "driverlib\cpu.h"
#include "driverlib\sysctl.h"
#include "driverlib\systick.h"
#include "driverlib\uart.h"
#include "driverlib\i2c.h"

#include "utils\uartstdio.h"

#include "inc\hw_gpio.h"
#include "inc\hw_ints.h"
#include "inc\hw_memmap.h"
#include "inc\hw_nvic.h"
#include "inc\hw_sysctl.h"
#include "inc\hw_types.h"
#include "inc\hw_uart.h"
#include "inc\hw_i2c.h"

#endif
