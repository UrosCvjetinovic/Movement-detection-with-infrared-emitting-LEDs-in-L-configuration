#include "Application.h"
#include "Common.h"
#include "UserFeedback.h"
#include "si115xRegisters.h"
#include "ProximityClickApi.h"

static uint8_t Si115xReadFromRegister(uint8_t ui8SensorAddr, uint8_t ui8RegAddr)
{
    uint8_t ui8Data = 0;

    I2CMasterSlaveAddrSet(I2C0_BASE, ui8SensorAddr, false);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));
    SysCtlDelay(10);

    I2CMasterDataPut(I2C0_BASE, ui8RegAddr);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while(I2CMasterBusy(I2C0_BASE));
    SysCtlDelay(10);

    I2CMasterSlaveAddrSet(I2C0_BASE, ui8SensorAddr, true);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
    while(I2CMasterBusy(I2C0_BASE));
    SysCtlDelay(10);
    ui8Data = I2CMasterDataGet(I2C0_BASE);

    return ui8Data;
}

static void Si115xWriteToRegister(uint8_t ui8SensorAddr, uint8_t ui8RegAddr, uint8_t ui8Data)
{
    I2CMasterSlaveAddrSet(I2C0_BASE, ui8SensorAddr, false);
    I2CMasterDataPut(I2C0_BASE, ui8RegAddr);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while(I2CMasterBusy(I2C0_BASE));
    SysCtlDelay(10);

    I2CMasterDataPut(I2C0_BASE, ui8Data);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C0_BASE));
    SysCtlDelay(10);
}

void Si115xReset(uint8_t ui8SensorAddr)
{
    SysCtlDelay(PROXIMITY_STARTUP_TIME * (SysCtlClockGet() / 3 / 1000));
    Si115xWriteToRegister(ui8SensorAddr, SI115x_REG_COMMAND, CMD_RESET);
    SysCtlDelay(PROXIMITY_STARTUP_TIME * (SysCtlClockGet() / 3 / 1000));
    SysCtlDelay(PROXIMITY_STARTUP_TIME * (SysCtlClockGet() / 3 / 1000));
}

static uint8_t Si115xGetPartId (uint8_t ui8SensorAddr)
{
    return Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_PART_ID);
}

static uint8_t Si115xGetHwId (uint8_t ui8SensorAddr)
{
    return Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_HW_ID);
}

static uint8_t Si115xGetRevId (uint8_t ui8SensorAddr)
{
    return Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_REV_ID);
}

static uint8_t Si115xGetResponse0(uint8_t ui8SensorAddr)
{
    return Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE0);
}

static uint8_t Si115xGetResponse1(uint8_t ui8SensorAddr)
{
    return Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE1);
}

static int16_t _waitUntilSleep(uint8_t ui8SensorAddr)
{
    int16_t ui16RetVal = -1;
    uint8_t ui8Count = 0;

    while (ui8Count < 5) {
        ui16RetVal = Si115xGetResponse0(ui8SensorAddr);
        if((ui16RetVal & RSP0_CHIPSTAT_MASK) == RSP0_SLEEP)
            break;
        if(ui16RetVal <  0)
            return ui16RetVal;
        ui8Count++;
    }
    return 0;
}

static int16_t _sendCmd(uint8_t ui8SensorAddr, uint8_t ui8Command)
{
    int16_t  ui16Response;
    int8_t   ui8RetVal;
    uint8_t  ui8Count = 0;

    UARTprintf("\n----- send CMD -----");

    ui16Response = Si115xGetResponse0(ui8SensorAddr);

    UARTprintf("\nRESPONSE 0 = ");
    write_hex(ui16Response);
    if(ui16Response < 0) {
        return ui16Response;
    }

    ui16Response = ui16Response & RSP0_COUNTER_MASK;

    UARTprintf("\nRESPONSE 0 = ");
    write_hex(ui16Response);

    while(ui8Count < 5) {
        if((ui8RetVal = _waitUntilSleep(ui8SensorAddr)) != 0)
            return ui8RetVal;

        if(ui8Command == 0)
            break;

        ui8RetVal = Si115xGetResponse0(ui8SensorAddr);

        if((ui8RetVal & RSP0_COUNTER_MASK) == ui16Response)
            break;
        else if(ui8RetVal < 0)
            return ui8RetVal;
        else
            ui16Response = ui8RetVal & RSP0_COUNTER_MASK;

        ui8Count++;
    }

    UARTprintf("\nRESPONSE 0 = ");
    write_hex(Si115xGetResponse0(ui8SensorAddr));

    UARTprintf("\n==Upisujem komandu");
    Si115xWriteToRegister(ui8SensorAddr, SI115x_REG_COMMAND, ui8Command);

    ui8Count = 0;

    while (ui8Count < 5) {
        if(ui8Command == 0)
            break;

        ui8RetVal = Si115xGetResponse0(ui8SensorAddr);
        UARTprintf("\nRESPONSE 0 = ");
        write_hex(ui8RetVal);

        if((ui8RetVal & RSP0_COUNTER_MASK) != ui16Response)
            break;
        else if(ui8RetVal < 0)
            return ui8RetVal;

        ui8Count++;
    }
    return 0;
}

static int16_t Si115xStart(uint8_t ui8SensorAddr)
{
    return _sendCmd(ui8SensorAddr, START);
}

static int16_t Si115xNop(uint8_t ui8SensorAddr)
{
    return _sendCmd(ui8SensorAddr, CMD_NOP);
}

static int16_t Si115xForce(uint8_t ui8SensorAddr)
{
    return _sendCmd(ui8SensorAddr, CMD_FORCE_CH);
}

static int16_t _Pause (uint8_t ui8SensorAddr)
{
    return _sendCmd(ui8SensorAddr, CMD_PAUSE_CH);
}

int16_t Si115xPause(uint8_t ui8SensorAddr)
{
    uint8_t ui8CountA, ui8CountB;
    int8_t  ui8RetVal;

    ui8RetVal = 0;
    while((RSP0_CHIPSTAT_MASK & ui8RetVal) != RSP0_SLEEP) {
        ui8RetVal = Si115xGetResponse0(ui8SensorAddr);
    }

    ui8CountA = 0;
    while(ui8CountA < 5) {
        ui8CountB = 0;
        while(ui8CountB < 5) {
            ui8RetVal = Si115xGetResponse0(ui8SensorAddr);
            if((ui8RetVal & RSP0_COUNTER_MASK) == 0)
                break;
            else {
                Si115xNop(ui8SensorAddr);
            }
            ui8CountB++;
        }

        _Pause(ui8SensorAddr);

        ui8CountB = 0;
        while(ui8CountB < 5) {
            ui8RetVal = Si115xGetResponse0(ui8SensorAddr);
            if((ui8RetVal & RSP0_COUNTER_MASK) != 0)
                break;
            ui8CountB++;
        }

        ui8RetVal = Si115xGetResponse0(ui8SensorAddr);
        if((ui8RetVal & RSP0_COUNTER_MASK) == 1 )
            break;
        ui8CountA++;
    }
    return 0;
}

void Si115xParamSet(uint8_t ui8SensorAddr, uint8_t ui8AddrParam, uint8_t ui8Data)
{
    uint8_t temp;

    UARTprintf("\n----- USO Param Set -----");
    temp = Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE0);
    UARTprintf("\nRESPONSE 0 = ");
    write_hex(temp);
    // Wait until sleep
    while (!(temp & RSP0_SLEEP)){
        UARTprintf("\nUSO u prvi while:");
        temp = Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE0);
    }

    UARTprintf("\nRESPONSE 0 = ");
    write_hex(temp);
    // CMD_ERR
    if(temp & 0x10) {
        UARTprintf("\nUSO u prvi if:");
        Si115xWriteToRegister(ui8SensorAddr, SI115x_REG_COMMAND, CMD_NOP);
        while(temp & 0xDF) {
            UARTprintf("\nUSO u drugi while:");
            temp = Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE0);
        }
    }

    UARTprintf("\nRESPONSE 0 = ");
    write_hex(temp);

    ui8AddrParam = CMD_PARAM_SET | (ui8AddrParam & 0x3F);

    UARTprintf("\nCOMMAND =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_COMMAND));
    UARTprintf("\nHOSTIN0 =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_HOSTIN0));

    Si115xWriteToRegister(ui8SensorAddr, HOSTIN_0_REG, ui8Data);
    Si115xWriteToRegister(ui8SensorAddr, COMMAND_REG, ui8AddrParam);

    UARTprintf("\nCOMMAND =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_COMMAND));
    UARTprintf("\nHOSTIN0 =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_HOSTIN0));

    UARTprintf("\nRESPONSE 0 = ");
    write_hex(temp);

    while( (temp & RSP0_COUNTER_MASK) == (Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE0) & 0x1f) );
    UARTprintf("\n----IZASAO IZ PARAM SET----");

}

uint8_t Si115xParamRead(uint8_t ui8SensorAddr, uint8_t ui8AddrParam)
{
    uint8_t temp;

    UARTprintf("\n-----USO Param READ------");

    temp = Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE0);
    UARTprintf("\nRESPONSE 0 = ");
    write_hex(temp);
    while (!(temp & RSP0_SLEEP)) {
        temp = Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE0);
        UARTprintf("\nUSO u prvi while:");
    }
    UARTprintf("\nRESPONSE 0 = ");
    write_hex(temp);
    if(temp & 0x10) {
        UARTprintf("\nUSO u prvi if:");
        Si115xWriteToRegister(ui8SensorAddr, SI115x_REG_COMMAND, CMD_NOP);
        while(temp & 0xDF) {
            UARTprintf("\nUSO u drugi while:");
            temp = Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE0);
        }
    }

    UARTprintf("\nRESPONSE 0 = ");
    write_hex(temp);

    ui8AddrParam = CMD_PARAM_QUERY + (ui8AddrParam & 0x3F);

    UARTprintf("\nCOMMAND =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_COMMAND));
    UARTprintf("\nHOSTIN0 =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_HOSTIN0));

    Si115xWriteToRegister(ui8SensorAddr, COMMAND_REG, ui8AddrParam);

    UARTprintf("\nCOMMAND =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_COMMAND));
    UARTprintf("\nHOSTIN0 =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_HOSTIN0));

    UARTprintf("\nRESPONSE 0 = ");
    write_hex(temp);

    while( (temp & RSP0_COUNTER_MASK) == (Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE0) & 0x1f) );

    UARTprintf("\n----IZASAO IZ PARAM READ----");
    return Si115xReadFromRegister(ui8SensorAddr, SI115x_REG_RESPONSE1);
}

void Setup(void)
{
    Si115xNop(PROXIMITY_ADR);
    UARTprintf("\nCOMMAND =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_COMMAND));
    UARTprintf("\nHOSTIN0 =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_HOSTIN0));
    UARTprintf("\nRESPONSE0 =");
    write_hex(Si115xReadFromRegister(PROXIMITY_ADR, SI115x_REG_RESPONSE0));

    UARTprintf("\nUSO:");
    UARTprintf("\nParam read:");
    write_hex(Si115xParamRead(PROXIMITY_ADR, CHAN_LIST));
    UARTprintf("\nPROSO:");

    UARTprintf("\nUSO:");
    Si115xParamSet(PROXIMITY_ADR, CHAN_LIST, 0x07);
    UARTprintf("\nParam read (RESPONSE1 = ");
    write_hex(Si115xParamRead(PROXIMITY_ADR, CHAN_LIST));
    UARTprintf("\nPROSO:");

    //Si115xParamSet(PROXIMITY_ADR, ADCCONFIG_0, SMALL_IR);
    Si115xParamSet(PROXIMITY_ADR, ADCCONFIG_0, 0);
    Si115xParamSet(PROXIMITY_ADR, MEASCONFIG_0, MEASCOUNT_0);

    //Si115xParamSet(PROXIMITY_ADR, ADCCONFIG_1, MEDIUM_IR);
    Si115xParamSet(PROXIMITY_ADR, ADCCONFIG_1, 1);
    Si115xParamSet(PROXIMITY_ADR, MEASCONFIG_1, MEASCOUNT_1);

    //Si115xParamSet(PROXIMITY_ADR, ADCCONFIG_2, LARGE_IR);
    Si115xParamSet(PROXIMITY_ADR, ADCCONFIG_2, 2);
    Si115xParamSet(PROXIMITY_ADR, MEASCONFIG_2, MEASCOUNT_2);

}

uint16_t ReadOutput(uint8_t ui8Addr)
{
    uint16_t ch = 0x0000;

    ch = Si115xReadFromRegister(PROXIMITY_ADR, HOSTOUT_0 + ui8Addr);
    ch = Si115xReadFromRegister(PROXIMITY_ADR, HOSTOUT_0 + 1 + ui8Addr) << 8;
    UARTprintf("\nREAD OUTPUT:");
    write_hex(ch);
    UARTprintf(" ");

    return ch;
}

void Start(void)
{
    Si115xStart(PROXIMITY_ADR);
}


uint32_t GetSpecs(void)
{
    uint32_t ui32HwConfig = 0;

    ui32HwConfig |= Si115xGetRevId(PROXIMITY_ADR);
    ui32HwConfig |= (Si115xGetHwId(PROXIMITY_ADR) << 8);
    ui32HwConfig |= (Si115xGetPartId(PROXIMITY_ADR)<< 16);

    return ui32HwConfig;
}







