#include <e_skate_bms.h>

#include <esp_err.h>
#include <driver/uart.h>


e_skate_err_t e_skate_bms_init(

    e_skate_bms_config_t* bmsConfig

)
{
    ESP_ERROR_CHECK(uart_param_config(
        bmsConfig->bmsUartPort,
        &bmsConfig->uartConfig
        ));

    ESP_ERROR_CHECK(uart_set_pin(
        bmsConfig->bmsUartPort,
        bmsConfig->batTxPin,
        bmsConfig->batRxPins[0],
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
        ));

    ESP_ERROR_CHECK(uart_driver_install(
        bmsConfig->bmsUartPort,
        E_SKATE_UART_BMS_BUFF_SIZE,
        0,
        0,
        NULL,
        0
        ));

    return E_SKATE_SUCCESS;
}