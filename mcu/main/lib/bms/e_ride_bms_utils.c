#include <esk8_err.h>
#include <esk8_bms.h>

esk8_err_t get_data_with_response(
    
    esk8_bms_config_t *bmsConfig,
    esk8_uart_addr_t   dstAddr,
    esk8_uart_reg_t    readReg,
    size_t                readRegSize,
    void                 *outValue

)
{
    esk8_err_t errorCode = ESK8_OK;
    esk8_uart_msg_t msg;

    size_t responseLen = ESK8_MSG_MIN_SIZE + readRegSize; // Payload size.
    uint8_t responseBuffer[responseLen];

    esk8_uart_regread_msg_new(
        dstAddr,
        readReg,
        readRegSize,
        &msg
    );

    int retries = 0;
    while(retries++ < ESK8_UART_BMS_MSG_UPDATE_RETRIES)
    {
        size_t msgLen  = esk8_uart_msg_get_serialized_length(msg);
        uint8_t* msgBuf = malloc(msgLen);
        
        esk8_uart_msg_serialize(msg, msgBuf);

        // Discard all data in RX buffer
        uart_flush_input(
            bmsConfig->bmsUartPort);
        
        uart_write_bytes(
            bmsConfig->bmsUartPort,
            (const char*) msgBuf,
            msgLen);

        uart_wait_tx_done(
            bmsConfig->bmsUartPort,
            ESK8_UART_BMS_UPDATE_MS / portTICK_PERIOD_MS);

        free(msgBuf);

        int responseLenR = uart_read_bytes(
            bmsConfig->bmsUartPort,
            responseBuffer,
            responseLen,
            ESK8_UART_BMS_UPDATE_MS / portTICK_PERIOD_MS);

        if (responseLenR != responseLen)
        {
            errorCode = responseLenR<=0?ESK8_BMS_ERR_NO_RESPONSE:ESK8_BMS_ERR_INVALID_LEN;
            continue;
        }

        esk8_uart_msg_t rspMsg;
        
        errorCode = esk8_uart_msg_parse(
            responseBuffer,
            responseLen,
            &rspMsg);

        if (errorCode != ESK8_OK)
        {
            continue;
        }

        if (rspMsg.src_address != ESK8_ADDR_BMS)
        {
            esk8_uart_msg_free(rspMsg);
            errorCode = ESK8_BMS_ERR_WRONG_ADDRESS;
            continue;
        }

        if (rspMsg.cmd_argment != readReg)
        {
            esk8_uart_msg_free(rspMsg);
            errorCode = ESK8_BMS_ERR_WRONG_RESPONSE;
            continue;
        }

        memcpy(outValue, rspMsg.payload, readRegSize);

        esk8_uart_msg_free(rspMsg);
        break;
    }

    esk8_uart_msg_free(msg);
    return errorCode;
}