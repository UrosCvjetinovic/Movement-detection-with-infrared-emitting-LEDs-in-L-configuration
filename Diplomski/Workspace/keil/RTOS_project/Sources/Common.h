#ifndef COMMON_H_
#define COMMON_H_

#define ITEM_SIZE           sizeof(uint8_t)
#define QUEUE_SIZE          5
#define COMTASKSTACKSIZE           128
#define READTASKSTACKSIZE           128
#define GESTURETASKSTACKSIZE           128

#define PROXIMITY_ADR 0x53
#define PROXIMITY_STARTUP_TIME      25 //ms

//Register addr
#define PART_ID_REG     0x00
#define HW_ID_REG       0x01
#define REV_ID_REG      0x02
#define HOSTIN_0_REG    0x0A
#define COMMAND_REG     0x0B
#define IRQENABLE_REG   0x0F
#define RESPONSE_1_REG  0x10
#define RESPONSE_0_REG  0x11
#define IRQSTATUS_REG   0x12
#define HOSTOUT_0       0x13

//Command codes
#define RESET_CMD_CTR   0x00
#define RESET_SW        0x01
#define FORCE           0x11
#define PAUSE           0x12
#define START           0x13

//Parameter addr
#define I2C_ADDR        0x00
#define CHAN_LIST       0x01
#define ADCCONFIG_0     0x02
#define ADCSENS_0       0x03
#define ADCPOST_0       0x04
#define MEASCONFIG_0    0x05
#define ADCCONFIG_1     0x06
#define ADCSENS_1       0x07
#define ADCPOST_1       0x08
#define MEASCONFIG_1    0x09
#define ADCCONFIG_2     0x0A
#define ADCSENS_2       0x0B
#define ADCPOST_2       0x0C
#define MEASCONFIG_2    0x0D


//Photodiodes
#define SMALL_IR    0b00000
#define MEDIUM_IR   0b00001
#define LARGE_IR    0b00010
#define WHITE       0b01011
#define LARGE_WHITE 0b01101

//Setup
#define MEASCOUNT_0 0x40
#define MEASCOUNT_1 0x80
#define MEASCOUNT_2 0xC0



#endif /* COMMON_H_ */
