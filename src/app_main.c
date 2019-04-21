#include <e_skate_uart.h>
#include <e_skate_config.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>
#include <esp_log.h>

#include <stdint.h>
#include <stdio.h>


void app_main()
{
    uart_config_t uartConfig = E_SKATE_UART_CONFIG_DEFAULT_ESP32();

    e_skate_uart_msg_t msg;
    e_skate_uart_msg_err_t errCode;

    ESP_ERROR_CHECK(uart_param_config(
        E_SKATE_UART_NUM,
        &uartConfig
        ));

    ESP_ERROR_CHECK(uart_set_pin(
        E_SKATE_UART_NUM,
        E_SKATE_UART_BMS_TX,
        E_SKATE_UART_BMS_RX,
        UART_PIN_NO_CHANGE,
        UART_PIN_NO_CHANGE
        ));
    
    ESP_ERROR_CHECK(uart_driver_install(
        E_SKATE_UART_NUM,
        E_SKATE_UART_BMS_BUFF_SIZE,
        0,
        0,
        NULL,
        0
        ));

    uint8_t rx_buffer[E_SKATE_UART_BMS_BUFF_SIZE];

    while(1)
    {
        errCode = e_skate_uart_regread_msg_new(
            E_SKATE_REG_BMS_TEMPRTR,
            0x02,
            &msg);
        
        if (errCode != E_SKATE_UART_MSG_SUCCESS)
            continue;

        size_t bufferSize = e_skate_uart_msg_get_serialized_length(msg);
        uint8_t  *buffer = (uint8_t*)malloc(bufferSize);

        e_skate_uart_msg_serialize(msg, buffer);
        e_skate_uart_msg_free(msg);

        uart_write_bytes(E_SKATE_UART_NUM, (const char*)  buffer, bufferSize);
        int len = uart_read_bytes(E_SKATE_UART_NUM, rx_buffer, E_SKATE_UART_BMS_BUFF_SIZE, 200 / portTICK_RATE_MS);

        if (len > 0)
        {
            printf("Got: %d bytes.\n", len);
            for (int i=0; i<len; i++)
                printf("0x%02x ", rx_buffer[i]);
            printf("\n");
        }

        free(buffer);
    }
}
