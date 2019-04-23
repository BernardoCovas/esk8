#include <e_skate_config.h>
#include <e_skate_uart.h>
#include <e_skate_bms.h>

#include <driver/uart.h>


e_skate_err_t e_skate_bms_init_from_config_h(

    e_skate_bms_config_t* outConfig

)
{
    static uint8_t uartPorts[] = E_SKATE_UART_BMS_RX_PINS;
    const uint8_t numBat       = sizeof(uartPorts) / sizeof(uint8_t);

    e_skate_bms_config_t bmsConfig = {
        .bmsUartPort = E_SKATE_UART_BMS_NUM,
        .numBat = numBat,
        .batTxPin = E_SKATE_UART_BMS_TX,
        .batRxPins = uartPorts,
        .uartConfig = E_SKATE_UART_CONFIG_DEFAULT_ESP32(),
        .bmsUpdateMs = E_SKATE_UART_BMS_UPDATE_MS
    };

    (*outConfig) = bmsConfig;

    return e_skate_bms_init(&bmsConfig);
}