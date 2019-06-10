#include <e_ride_config.h>
#include <e_ride_bms.h>
#include <e_ride_bms_utils.h>
#include <e_ride_uart.h>

#include <driver/uart.h>


e_ride_err_t e_ride_bms_get_status(

    e_ride_bms_config_t *bmsConfig,
    e_ride_bms_status_t *outStatus

)
{
    e_ride_err_t errorCode = E_RIDE_SUCCESS;
    
    e_ride_uart_reg_t  readReg[] = {
        E_RIDE_REG_BMS_CAPACITY,
        E_RIDE_REG_BMS_VOLTAGE,
        E_RIDE_REG_BMS_CURRENT,
        E_RIDE_REG_BMS_TEMPRTR,
    };

    uint8_t numRegs = sizeof(readReg) / sizeof(e_ride_uart_reg_t);
    uint16_t outValues[numRegs];


    for (int i = 0; i < numRegs; i++)
    {
        errorCode = get_data_with_response(
            bmsConfig,
            E_RIDE_ADDR_BMS,
            readReg[i],
            2,
            &outValues[i]);

        if (errorCode != E_RIDE_SUCCESS)
            return errorCode;
    }

    memcpy(outStatus, outValues, sizeof(outValues[0]) * numRegs);

    return errorCode;
}