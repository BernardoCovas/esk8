#include <e_skate_bms.h>
#include <e_skate_config.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/uart.h>
#include <esp_log.h>

#include <stdint.h>
#include <stdio.h>


static void uart_intr_handle(void *arg)
{
//     size_t rawBufLen;
//     e_skate_bms_err_t errCode;
//     e_skate_bms_msg_t newMsg;
//
//     uart_write_bytes(UART_NUM_0, "Got packet.\n", 13); 
// 
//     uart_get_buffered_data_len(E_SKATE_UART_NUM, &rawBufLen);
//     
//     uint8_t* rawBuf = (uint8_t*)malloc(rawBufLen);
//     uart_read_bytes(E_SKATE_UART_NUM, rawBuf, rawBufLen, 100 / portTICK_PERIOD_MS);
// 
//     errCode = e_skate_bms_msg_parse(rawBuf, rawBufLen, &newMsg);
//     
//     if (errCode != E_SKATE_BMS_MSG_SUCCESS)
//         goto GOTO_HLDR_DONE;
// 
//     printf("Got message.\n");    
//     e_skate_bms_msg_free(newMsg);
//     
       uart_clear_intr_status(E_SKATE_UART_NUM, UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);
// 
//     goto GOTO_HLDR_DONE;
// 
// GOTO_HLDR_DONE:
//     free(rawBuf);
}

void app_main()
{
    uart_config_t uartConfig = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE
	};

    uart_isr_handle_t handle;
    e_skate_bms_msg_t msg;
    e_skate_bms_err_t errCode;

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
    
    ESP_ERROR_CHECK(uart_driver_install(E_SKATE_UART_NUM, UART_FIFO_LEN*1000, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_isr_free(E_SKATE_UART_NUM));
    
    ESP_ERROR_CHECK(uart_isr_register(E_SKATE_UART_NUM, &uart_intr_handle, NULL, ESP_INTR_FLAG_LOWMED, &handle));
    ESP_ERROR_CHECK(uart_enable_rx_intr(E_SKATE_UART_NUM));

    while(1)
    {
        printf("app_main()\n");
        errCode = e_skate_bms_msg_new(E_SKATE_BMS_REG_CURRENT, &msg);
        
        if (errCode != E_SKATE_BMS_MSG_SUCCESS)
            continue;

        size_t bufferSize = e_skate_bms_msg_get_serialized_length(msg);
        uint8_t  *buffer = (uint8_t*)malloc(bufferSize);

        e_skate_bms_msg_serialize(msg, buffer);
        e_skate_bms_msg_free(msg);

        ESP_ERROR_CHECK(uart_enable_rx_intr(E_SKATE_UART_NUM));
        uart_write_bytes(E_SKATE_UART_NUM, (const char*)  buffer, bufferSize);

        free(buffer),
        vTaskDelay(200 / portTICK_PERIOD_MS);    
    }
}
