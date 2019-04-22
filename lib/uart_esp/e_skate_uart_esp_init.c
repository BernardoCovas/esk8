#include <e_skate_config.h>
#include <e_skate_uart_esp.h>

#include <esp_err.h>
#include <driver/uart.h>


e_skate_uart_esp_err_t e_skate_uart_esp_bms_init(
    e_skate_uart_esp_bms_t* eSkateUart,
    uart_port_t         uartPort,
    uint8_t             uartRxI
)
{
    const static gpio_num_t eSkateBmsPins[] = E_SKATE_UART_BMS_RX_PINS;
    const uint8_t numPins = sizeof(eSkateBmsPins) / sizeof(gpio_num_t);
    uart_config_t uartConfig = E_SKATE_UART_CONFIG_DEFAULT_ESP32();

    if (uartRxI >= numPins)
        return E_SKATE_UART_BAD_PARAM;

    ESP_ERROR_CHECK(uart_param_config(
        uartPort,
        &uartConfig
        ));

    ESP_ERROR_CHECK(uart_set_pin(
        uartPort,
        E_SKATE_UART_BMS_TX,
        eSkateBmsPins[uartRxI],
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
        ));

    ESP_ERROR_CHECK(uart_driver_install(
        uartPort,
        E_SKATE_UART_BMS_BUFF_SIZE,
        0,
        0,
        NULL,
        0
        ));

    eSkateUart->curBmsPin   = uartRxI;
    eSkateUart->numBat      = numPins;
    eSkateUart->uartNum     = uartPort;
    eSkateUart->msg_cb      = NULL;

    return E_SKATE_UART_SUCCESS;
}