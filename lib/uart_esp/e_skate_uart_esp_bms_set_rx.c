#include <e_skate_config.h>
#include <e_skate_uart_esp.h>

#include <esp_err.h>
#include <driver/uart.h>


e_skate_uart_esp_err_t e_skate_uart_esp_bms_set_rx(
    e_skate_uart_esp_bms_t* eSkateUart,
    uint8_t uartRxI
)
{
    const static gpio_num_t eSkateBmsPins[] = E_SKATE_UART_BMS_RX_PINS;

    if (uartRxI > eSkateUart->numBat)
        return E_SKATE_UART_BAD_PARAM;

    eSkateUart->curBmsPin = uartRxI;

    ESP_ERROR_CHECK(uart_set_pin(
        eSkateUart->uartNum,
        E_SKATE_UART_BMS_TX,
        eSkateBmsPins[eSkateUart->curBmsPin],
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
        ));

    return E_SKATE_UART_SUCCESS;
}