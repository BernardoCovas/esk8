#include <e_ride_config.h>
#include <e_ride_err.h>
#include <e_ride_bms.h>

#include <esp_err.h>
#include <driver/uart.h>


e_ride_err_t e_ride_bms_set_rx(

    e_ride_bms_config_t* bmsConfig,
    uint8_t uartRxI

)
{
    if (uartRxI > bmsConfig->numBat)
        return E_RIDE_ERR_INVALID_PARAM;

    ESP_ERROR_CHECK(uart_set_pin(
        bmsConfig->bmsUartPort,
        bmsConfig->batTxPins[uartRxI],
        bmsConfig->batRxPins[uartRxI],
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
        ));

    return E_RIDE_SUCCESS;
}
