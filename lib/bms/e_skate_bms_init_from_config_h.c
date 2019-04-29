#include <e_skate_config.h>
#include <e_skate_uart.h>
#include <e_skate_bms.h>

#include <driver/uart.h>


e_skate_err_t e_skate_bms_init_from_config_h(

    e_skate_bms_config_t* outConfig

)
{
    static uint8_t uartRxPorts[] = E_SKATE_UART_BMS_RX_PINS;
    static uint8_t uartTxPorts[] = E_SKATE_UART_BMS_TX_PINS;

    const uint8_t numRx       = sizeof(uartRxPorts) / sizeof(uint8_t);
    const uint8_t numTx       = sizeof(uartTxPorts) / sizeof(uint8_t);

    if (numRx != numTx)
        return E_SKATE_ERR_INVALID_PARAM;

    e_skate_bms_config_t bmsConfig = {
        .bmsUartPort = E_SKATE_UART_BMS_NUM,
        .numBat = numRx,
        .batTxPins = uartTxPorts,
        .batRxPins = uartRxPorts,
        .uartConfig = E_SKATE_UART_CONFIG_DEFAULT_ESP32(),
        .bmsUpdateMs = E_SKATE_UART_BMS_UPDATE_MS
    };

    (*outConfig) = bmsConfig;

    return e_skate_bms_init(&bmsConfig);
}