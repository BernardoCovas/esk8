#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_bms.h>

#include <esp_err.h>
#include <driver/uart.h>


esk8_err_t esk8_bms_set_rx(

    esk8_bms_config_t* bmsConfig,
    uint8_t uartRxI

)
{
    if (uartRxI > bmsConfig->numBat)
        return ESK8_ERR_INVALID_PARAM;

    ESP_ERROR_CHECK(uart_set_pin(
        bmsConfig->bmsUartPort,
        bmsConfig->batTxPins[uartRxI],
        bmsConfig->batRxPins[uartRxI],
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
        ));

    return ESK8_OK;
}
