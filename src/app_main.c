#include <e_skate_bms.h>

#include <driver/uart.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdint.h>
#include <stdio.h>

static void IRAM_ATTR uart_intr_handle(void *arg)
{
    uint16_t rx_fifo_len, status;
    uint16_t i;

    status = UART0.int_st.val; // read UART interrupt Status

    rx_fifo_len = UART0.status.rxfifo_cnt; // read number of bytes in UART buffer
    uint8_t *msgBuff = (uint8_t*)malloc(rx_fifo_len);

    while(rx_fifo_len){
    msgBuff[i++] = UART0.fifo.rw_byte; // read all bytes
    rx_fifo_len--;
    }

    // after reading bytes from buffer clear UART interrupt status
    uart_clear_intr_status(UART_NUM_1, UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);

    e_skate_bms_msg_t newMsg;
    e_skate_bms_err_t errCode = e_skate_bms_msg_parse(msgBuff, rx_fifo_len, &newMsg);

    free(msgBuff);
}

void app_main()
{
    uart_config_t uartConfig;
    uart_isr_handle_t handle;

    uart_isr_register(UART_NUM_1, &uart_intr_handle, NULL, 0, &handle);
    uart_enable_rx_intr(UART_NUM_1);

}