#include <esk8_bms.h>

#include <esp_err.h>
#include <driver/uart.h>


esk8_err_t
esk8_bms_init(
    esk8_bms_config_t* bms_cnfg,
    esk8_bms_hndl_t* out_hndl
)
{
    esp_err_t err;
    esk8_err_t esk8_err;
    uart_config_t uart_cnfg = ESK8_UART_CONFIG_DEFAULT_ESP32();

    *out_hndl = calloc(1, sizeof(esk8_bms_config_t));
    if (!(*out_hndl))
        return ESK8_ERR_OOM;

    **(esk8_bms_config_t**)out_hndl = *bms_cnfg;

    err = uart_param_config(
        bms_cnfg->uart_port,
        &uart_cnfg
    );

    if (err)
    {
        return ESK8_ERR_INVALID_PARAM;
        goto fail;
    }

    bms_cnfg->tx_pin = 0;

    err = uart_set_pin(
        bms_cnfg->uart_port,
        bms_cnfg->tx_pins[0],
        bms_cnfg->rx_pins[0],
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
    );

    if (err)
    {
        esk8_err = ESK8_ERR_INVALID_PARAM;
        goto fail;
    }

    err = uart_driver_install(
        bms_cnfg->uart_port,
        ESK8_UART_BMS_BUFF_SIZE,
        0, 0, NULL, 0
    );

    if (err)
    {
        esk8_err = ESK8_ERR_INVALID_PARAM;
        goto fail;
    }

    /**
     * BLE (and others) communicate through UART,
     * so this might already be installed. In that
     * case it returns ESP_FAIL (Instead of a specific error),
     * and we can ignore it.
     */
    if (err != ESP_OK && err != ESP_FAIL)
    {
        err = ESK8_ERR_INVALID_PARAM;
        goto fail;
    }

    return ESK8_OK;

fail:
    free(*out_hndl);
    return esk8_err;
}
