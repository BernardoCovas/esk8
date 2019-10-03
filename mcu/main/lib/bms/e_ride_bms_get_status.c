#include <esk8_config.h>
#include <esk8_bms.h>
#include <esk8_bms_utils.h>
#include <esk8_uart.h>

#include <driver/uart.h>


esk8_err_t esk8_bms_get_status(

    esk8_bms_config_t *bmsConfig,
    esk8_bms_status_t *outStatus

)
{
    esk8_err_t errorCode = ESK8_OK;

    esk8_uart_reg_t  readReg[] = {
        ESK8_REG_BMS_CAPACITY,
        ESK8_REG_BMS_VOLTAGE,
        ESK8_REG_BMS_CURRENT,
        ESK8_REG_BMS_TEMPRTR,
    };

    uint8_t numRegs = sizeof(readReg) / sizeof(esk8_uart_reg_t);
    uint16_t outValues[numRegs];


    for (int i = 0; i < numRegs; i++)
    {
        errorCode = get_data_with_response(
            bmsConfig,
            ESK8_ADDR_BMS,
            readReg[i],
            2,
            &outValues[i]);

        if (errorCode != ESK8_OK)
            return errorCode;
    }

    memcpy(outStatus, outValues, sizeof(outValues[0]) * numRegs);

    return errorCode;
}