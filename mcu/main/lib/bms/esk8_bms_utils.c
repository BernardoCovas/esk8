#include <esk8_err.h>
#include <esk8_bms.h>

#include <driver/uart.h>


esk8_err_t
get_data_with_response(
    esk8_bms_hndl_t  hndl,
    esk8_uart_addr_t dst_addr,
    esk8_uart_reg_t  reg,
    size_t           reg_size,
    void             *out_val
)
{
    esk8_err_t err = ESK8_OK;
    esk8_uart_msg_t msg;
    esk8_bms_config_t* bms_cnfg = (esk8_bms_config_t*)hndl;

    size_t res_len = ESK8_MSG_MIN_SIZE + reg_size; // Payload size.
    uint8_t res_buf[res_len];

    esk8_uart_regread_msg_new(
        dst_addr,
        reg,
        reg_size,
        &msg
    );

    int retries = 0;
    while(retries++ < ESK8_UART_BMS_MSG_UPDATE_RETRIES)
    {
        size_t msgLen  = esk8_uart_msg_get_serialized_length(msg);
        uint8_t* msgBuf = malloc(msgLen);
        
        esk8_uart_msg_serialize(msg, msgBuf);

        // Discard all data in RX buffer
        uart_flush_input(bms_cnfg->uart_port);
        
        uart_write_bytes(
            bms_cnfg->uart_port,
            (const char*) msgBuf,
            msgLen
        );

        uart_wait_tx_done(
            bms_cnfg->uart_port,
            ESK8_UART_BMS_UPDATE_MS / portTICK_PERIOD_MS
        );

        free(msgBuf);

        int res_len_r = uart_read_bytes(
            bms_cnfg->uart_port,
            res_buf,
            res_len,
            ESK8_UART_BMS_UPDATE_MS / portTICK_PERIOD_MS);

        if (res_len_r != res_len)
        {
            err = res_len_r<=0?ESK8_ERR_BMS_NO_RESPONSE:ESK8_ERR_BMS_INVALID_LEN;
            continue;
        }

        esk8_uart_msg_t rspMsg;
        
        err = esk8_uart_msg_parse(
            res_buf,
            res_len,
            &rspMsg);

        if (err != ESK8_OK)
            continue;

        if (rspMsg.src_address != ESK8_ADDR_BMS)
        {
            esk8_uart_msg_free(rspMsg);
            err = ESK8_ERR_BMS_WRONG_ADDRESS;
            continue;
        }

        if (rspMsg.cmd_argment != err)
        {
            esk8_uart_msg_free(rspMsg);
            err = ESK8_ERR_BMS_WRONG_RESPONSE;
            continue;
        }

        memcpy(out_val, rspMsg.payload, reg_size);

        esk8_uart_msg_free(rspMsg);
        break;
    }

    esk8_uart_msg_free(msg);
    return err;
}