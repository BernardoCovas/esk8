#include <e_ride_err.h>
#include <e_ride_bms.h>

e_ride_err_t get_data_with_response(
    
    e_ride_bms_config_t *bmsConfig,
    e_ride_uart_addr_t   dstAddr,
    e_ride_uart_reg_t    readReg,
    size_t                readRegSize,
    void                 *outValue

)
{
    e_ride_err_t errorCode = E_RIDE_SUCCESS;
    e_ride_uart_msg_t msg;

    size_t responseLen = E_RIDE_MSG_MIN_SIZE + readRegSize; // Payload size.
    uint8_t responseBuffer[responseLen];

    e_ride_uart_regread_msg_new(
        dstAddr,
        readReg,
        readRegSize,
        &msg
    );

    int retries = 0;
    while(retries++ < E_RIDE_UART_BMS_MSG_UPDATE_RETRIES)
    {
        size_t msgLen  = e_ride_uart_msg_get_serialized_length(msg);
        uint8_t* msgBuf = malloc(msgLen);
        
        e_ride_uart_msg_serialize(msg, msgBuf);

        // Discard all data in RX buffer
        uart_flush_input(
            bmsConfig->bmsUartPort);
        
        uart_write_bytes(
            bmsConfig->bmsUartPort,
            (const char*) msgBuf,
            msgLen);

        uart_wait_tx_done(
            bmsConfig->bmsUartPort,
            E_RIDE_UART_BMS_UPDATE_MS / portTICK_PERIOD_MS);

        free(msgBuf);

        int responseLenR = uart_read_bytes(
            bmsConfig->bmsUartPort,
            responseBuffer,
            responseLen,
            E_RIDE_UART_BMS_UPDATE_MS / portTICK_PERIOD_MS);

        if (responseLenR != responseLen)
        {
            errorCode = responseLenR<=0?E_RIDE_BMS_ERR_NO_RESPONSE:E_RIDE_BMS_ERR_INVALID_LEN;
            continue;
        }

        e_ride_uart_msg_t rspMsg;
        
        errorCode = e_ride_uart_msg_parse(
            responseBuffer,
            responseLen,
            &rspMsg);

        if (errorCode != E_RIDE_SUCCESS)
        {
            continue;
        }

        if (rspMsg.src_address != E_RIDE_ADDR_BMS)
        {
            e_ride_uart_msg_free(rspMsg);
            errorCode = E_RIDE_BMS_ERR_WRONG_ADDRESS;
            continue;
        }

        if (rspMsg.cmd_argment != readReg)
        {
            e_ride_uart_msg_free(rspMsg);
            errorCode = E_RIDE_BMS_ERR_WRONG_RESPONSE;
            continue;
        }

        memcpy(outValue, rspMsg.payload, readRegSize);

        e_ride_uart_msg_free(rspMsg);
        break;
    }

    e_ride_uart_msg_free(msg);
    return errorCode;
}