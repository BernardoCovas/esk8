#include <esk8_err.h>


const char* esk8_err_to_str(
    esk8_err_t errCode
)
{
    switch(errCode)
    {
        case ESK8_SUCCESS: 
            return "ESK8_SUCCESS";
        case ESK8_ERR_INVALID_PARAM: 
            return "ESK8_ERR_INVALID_PARAM";
        case ESK8_ERR_OOM:
            return "ESK8_ERR_OOM";
        case ESK8_BMS_ERR_INVALID_DATA: 
            return "ESK8_BMS_ERR_INVALID_DATA";
        case ESK8_BMS_ERR_INVALID_LEN: 
            return "ESK8_BMS_ERR_INVALID_LEN";
        case ESK8_BMS_ERR_WRONG_ADDRESS: 
            return "ESK8_BMS_ERR_WRONG_ADDRESS";
        case ESK8_BMS_ERR_WRONG_RESPONSE: 
            return "ESK8_BMS_ERR_WRONG_RESPONSE";
        case ESK8_BMS_ERR_NO_RESPONSE: 
            return "ESK8_BMS_ERR_NO_RESPONSE";
        case ESK8_UART_MSG_ERR_NO_HEADER: 
            return "ESK8_UART_MSG_ERR_NO_HEADER";
        case ESK8_UART_MSG_ERR_INVALID_PLDLEN: 
            return "ESK8_UART_MSG_ERR_INVALID_PLDLEN";
        case ESK8_UART_MSG_ERR_INVALID_CHKSUM: 
            return "ESK8_UART_MSG_ERR_INVALID_CHKSUM";
        case ESK8_UART_MSG_ERR_INVALID_BUFFER: 
            return "ESK8_UART_MSG_ERR_INVALID_BUFFER";
        case ESK8_PS2_ERR_VALUE_READY:
            return "ESK8_PS2_ERR_VALUE_READY";
        case ESK8_PS2_ERR_INVALID_STATE:
            return "ESK8_PS2_ERR_INVALID_STATE";
        case ESK8_PS2_ERR_TIMEOUT:
            return "ESK8_PS2_ERR_TIMEOUT";
        default:
            return "?";
    }
}