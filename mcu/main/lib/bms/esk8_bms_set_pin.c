#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_bms.h>

#include <esp_err.h>
#include <driver/uart.h>


esk8_err_t
esk8_bms_set_pin(
    esk8_bms_hndl_t* hndl,
    uint8_t pin
)
{
    esk8_bms_config_t* bms_cnfg = (esk8_bms_config_t*)hndl;

    if (pin > bms_cnfg->bat_num)
        return ESK8_ERR_INVALID_PARAM;

    esp_err_t err = uart_set_pin(
        bms_cnfg->uart_port,
        bms_cnfg->tx_pins[pin],
        bms_cnfg->rx_pins[pin],
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
    );

    if (err)
        return ESK8_ERR_INVALID_PARAM;

    return ESK8_OK;
}
