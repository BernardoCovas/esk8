#include <esk8_config.h>
#include <esk8_bms.h>
#include <esk8_uart.h>

#include "esk8_bms_utils.h"

#include <driver/uart.h>


esk8_err_t
esk8_bms_get_status(
    esk8_bms_hndl_t hndl,
    esk8_bms_status_t *out_status
)
{
    esk8_err_t err = ESK8_OK;

    esk8_uart_reg_t  reg_list[] = {
        ESK8_REG_BMS_CAPACITY,
        ESK8_REG_BMS_VOLTAGE,
        ESK8_REG_BMS_CURRENT,
        ESK8_REG_BMS_TEMPRTR,
    };

    uint8_t num_regs = sizeof(reg_list) / sizeof(esk8_uart_reg_t);
    uint16_t out_values[num_regs];

    for (int i = 0; i < num_regs; i++)
    {
        err = get_data_with_response(
            hndl, ESK8_ADDR_BMS, reg_list[i],
            2, &out_values[i]
        );

        if (err != ESK8_OK)
            return err;
    }

    memcpy(out_status, out_values, sizeof(out_values[0]) * num_regs);
    return err;
}