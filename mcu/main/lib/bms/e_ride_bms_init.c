#include <e_ride_bms.h>

#include <esp_err.h>
#include <driver/uart.h>


e_ride_err_t e_ride_bms_init(

    e_ride_bms_config_t* bmsConfig

)
{
    ESP_ERROR_CHECK(uart_param_config(bmsConfig->bmsUartPort, &bmsConfig->uartConfig));

    ESP_ERROR_CHECK(uart_set_pin(
        bmsConfig->bmsUartPort, bmsConfig->batTxPin,
        bmsConfig->batRxPins[0], UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    esp_err_t errCode = uart_driver_install(
        bmsConfig->bmsUartPort, E_RIDE_UART_BMS_BUFF_SIZE, 0, 0, NULL, 0);

    /**
     * BLE (and others) communicate through UART,
     * so this might already be installed. In that
     * case it returns ESP_FAIL (Instead of a specific error),
     * and we can ignore it.
     */
    if (errCode != ESP_OK && errCode != ESP_FAIL)
        ESP_ERROR_CHECK(errCode);

    return E_RIDE_SUCCESS;
}