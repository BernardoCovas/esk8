#include <e_skate_config.h>
#include <e_skate_uart_esp.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>
#include <esp_log.h>

#include <stdint.h>
#include <stdio.h>


void app_main()
{
    e_skate_uart_msg_t msg;
    e_skate_uart_msg_err_t errCode;
    e_skate_uart_esp_bms_t espUart;

    e_skate_uart_esp_bms_init(
        &espUart,
        E_SKATE_UART_BMS_NUM,
        0);

    uint8_t rx_buffer[E_SKATE_UART_BMS_BUFF_SIZE];

    int count = 1;
    int currIndex = 0;
    while(1)
    {
        if (count % 10 == 0)
        {
            e_skate_uart_esp_bms_set_rx(&espUart, currIndex);
            currIndex ++;
            if (currIndex >= espUart.numBat)
                currIndex = 0;

            printf("Changed to index: %d\n", currIndex);
        }   

        errCode = e_skate_uart_regread_msg_new(
            E_SKATE_REG_BMS_VOLTAGE,
            0x02,
            &msg);
        
        if (errCode != E_SKATE_UART_MSG_SUCCESS)
            continue;

        size_t bufferSize = e_skate_uart_msg_get_serialized_length(msg);
        uint8_t  *buffer = (uint8_t*)malloc(bufferSize);

        e_skate_uart_msg_serialize(msg, buffer);
        e_skate_uart_msg_free(msg);

        uart_write_bytes(E_SKATE_UART_BMS_NUM, (const char*)  buffer, bufferSize);
        int len = uart_read_bytes(E_SKATE_UART_BMS_NUM, rx_buffer, E_SKATE_UART_BMS_BUFF_SIZE, 200 / portTICK_RATE_MS);

        if (len > 0)
        {
            printf("Got: %d bytes.\n", len);
            for (int i=0; i<len; i++)
                printf("0x%02x ", rx_buffer[i]);
            printf("\n");
        }

        free(buffer);
        count++;
    }
}
