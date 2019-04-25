#include <e_skate_config.h>
#include <e_skate_bms.h>
#include <e_skate_bms_utils.h>
#include <e_skate_uart.h>

#include <driver/uart.h>


e_skate_err_t e_skate_bms_get_status(

    e_skate_bms_config_t *bmsConfig,
    e_skate_bms_status_t *outStatus

)
{
    e_skate_err_t   errorCode = E_SKATE_SUCCESS;
    
    e_skate_uart_reg_t  readReg[] = {
        E_SKATE_REG_BMS_CAPACITY,
        E_SKATE_REG_BMS_VOLTAGE,
        E_SKATE_REG_BMS_CURRENT,
        E_SKATE_REG_BMS_TEMPRTR,
    };

    uint8_t numRegs = sizeof(readReg) / sizeof(e_skate_uart_reg_t);
    uint16_t outValues[numRegs];


    for (int i = 0; i < numRegs; i++)
    {
        errorCode = get_data_with_response(
            bmsConfig,
            E_SKATE_ADDR_BMS,
            readReg[i],
            2,
            &outValues[i]);

        if (errorCode != E_SKATE_SUCCESS)
            return errorCode;
    }

    memcpy(outStatus, outValues, sizeof(outValues[0]) * numRegs);

    return errorCode;
}