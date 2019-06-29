#include <esk8_config.h>
#include <esk8_uart.h>
#include <esk8_bms.h>

#include <driver/uart.h>
#include <driver/gpio.h>


esk8_err_t esk8_bms_init_from_config_h(

    esk8_bms_config_t* outConfig

)
{
    static uint8_t uartRxPorts[] = ESK8_UART_BMS_RX_PINS;
    static uint8_t uartTxPorts[] = ESK8_UART_BMS_TX_PINS;

    const uint8_t numRx       = sizeof(uartRxPorts) / sizeof(uint8_t);
    const uint8_t numTx       = sizeof(uartTxPorts) / sizeof(uint8_t);

    if (numRx != numTx)
        return ESK8_ERR_INVALID_PARAM;

    esk8_bms_config_t bmsConfig = {
        .bmsUartPort = ESK8_UART_BMS_NUM,
        .numBat = numRx,
        .batTxPins = uartTxPorts,
        .batRxPins = uartRxPorts,
        .uartConfig = ESK8_UART_CONFIG_DEFAULT_ESP32(),
        .bmsUpdateMs = ESK8_UART_BMS_UPDATE_MS
    };

    (*outConfig) = bmsConfig;

    return esk8_bms_init(&bmsConfig);
}