#include <esk8_config.h>
#include <esk8_uart.h>
#include <esk8_bms.h>

#include <driver/uart.h>
#include <driver/gpio.h>


esk8_err_t
esk8_bms_init_from_config_h(
    esk8_bms_hndl_t* out_hndl
)
{
    static uint8_t rx_pins[] = ESK8_UART_BMS_RX_PINS;
    static uint8_t tx_pins[] = ESK8_UART_BMS_TX_PINS;

    const uint8_t rx_num       = sizeof(rx_pins) / sizeof(uint8_t);
    const uint8_t tx_num       = sizeof(tx_pins) / sizeof(uint8_t);

    if (rx_num != tx_num)
        return ESK8_ERR_INVALID_PARAM;

    esk8_bms_config_t bms_cnfg = {
        .uart_port = ESK8_UART_BMS_NUM,
        .bat_num = rx_num,
        .tx_pins = tx_pins,
        .rx_pins = rx_pins,
        .bms_update_ms = ESK8_UART_BMS_UPDATE_MS
    };

    return esk8_bms_init(&bms_cnfg, out_hndl);
}