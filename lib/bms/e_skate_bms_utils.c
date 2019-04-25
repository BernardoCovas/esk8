#include <e_skate_err.h>
#include <e_skate_bms.h>

e_skate_err_t get_data_with_response(
    
    e_skate_bms_config_t *bmsConfig,
    e_skate_uart_addr_t   dstAddr,
    e_skate_uart_reg_t    readReg,
    size_t                readRegSize,
    void                 *outValue

)
{
    e_skate_err_t errorCode = E_SKATE_SUCCESS;
    e_skate_uart_msg_t msg;

    size_t responseLen = E_SKATE_MSG_MIN_SIZE + readRegSize; // Payload size.
    uint8_t responseBuffer[responseLen];

    e_skate_uart_regread_msg_new(
        dstAddr,
        readReg,
        readRegSize,
        &msg
    );

    int retries = 0;
    while(retries++ < E_SKATE_UART_BMS_MSG_UPDATE_RETRIES)
    {
        size_t msgLen  = e_skate_uart_msg_get_serialized_length(msg);
        uint8_t* msgBuf = malloc(msgLen);
        
        e_skate_uart_msg_serialize(msg, msgBuf);

        // Discard all data in RX buffer
        uart_flush_input(
            bmsConfig->bmsUartPort);
        
        uart_write_bytes(
            bmsConfig->bmsUartPort,
            (const char*) msgBuf,
            msgLen);

        uart_wait_tx_done(
            bmsConfig->bmsUartPort,
            E_SKATE_UART_BMS_UPDATE_MS / portTICK_PERIOD_MS);

        free(msgBuf);

        int responseLenR = uart_read_bytes(
            bmsConfig->bmsUartPort,
            responseBuffer,
            responseLen,
            E_SKATE_UART_BMS_UPDATE_MS / portTICK_PERIOD_MS);

        if (responseLenR != responseLen)
        {
            errorCode = responseLenR<=0?E_SKATE_BMS_ERR_NO_RESPONSE:E_SKATE_BMS_ERR_INVALID_LEN;
            continue;
        }

        e_skate_uart_msg_t rspMsg;
        
        errorCode = e_skate_uart_msg_parse(
            responseBuffer,
            responseLen,
            &rspMsg);

        if (errorCode != E_SKATE_SUCCESS)
        {
            continue;
        }

        if (rspMsg.src_address != E_SKATE_ADDR_BMS)
        {
            e_skate_uart_msg_free(rspMsg);
            errorCode = E_SKATE_BMS_ERR_WRONG_ADDRESS;
            continue;
        }

        if (rspMsg.cmd_argment != readReg)
        {
            e_skate_uart_msg_free(rspMsg);
            errorCode = E_SKATE_BMS_ERR_WRONG_RESPONSE;
            continue;
        }

        memcpy(outValue, rspMsg.payload, readRegSize);

        e_skate_uart_msg_free(rspMsg);
        break;
    }

    e_skate_uart_msg_free(msg);
    return errorCode;
}