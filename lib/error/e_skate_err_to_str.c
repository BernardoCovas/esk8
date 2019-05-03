#include <e_skate_err.h>


const char* e_skate_err_to_str(
    e_skate_err_t errCode
)
{
    switch(errCode)
    {
        case E_SKATE_SUCCESS: 
            return "E_SKATE_SUCCESS";
        case E_SKATE_ERR_INVALID_PARAM: 
            return "E_SKATE_ERR_INVALID_PARAM";
        case E_SKATE_ERR_OOM:
            return "E_SKATE_ERR_OOM";
        case E_SKATE_BMS_ERR_INVALID_DATA: 
            return "E_SKATE_BMS_ERR_INVALID_DATA";
        case E_SKATE_BMS_ERR_INVALID_LEN: 
            return "E_SKATE_BMS_ERR_INVALID_LEN";
        case E_SKATE_BMS_ERR_WRONG_ADDRESS: 
            return "E_SKATE_BMS_ERR_WRONG_ADDRESS";
        case E_SKATE_BMS_ERR_WRONG_RESPONSE: 
            return "E_SKATE_BMS_ERR_WRONG_RESPONSE";
        case E_SKATE_BMS_ERR_NO_RESPONSE: 
            return "E_SKATE_BMS_ERR_NO_RESPONSE";
        case E_SKATE_UART_MSG_ERR_NO_HEADER: 
            return "E_SKATE_UART_MSG_ERR_NO_HEADER";
        case E_SKATE_UART_MSG_ERR_INVALID_PLDLEN: 
            return "E_SKATE_UART_MSG_ERR_INVALID_PLDLEN";
        case E_SKATE_UART_MSG_ERR_INVALID_CHKSUM: 
            return "E_SKATE_UART_MSG_ERR_INVALID_CHKSUM";
        case E_SKATE_UART_MSG_ERR_INVALID_BUFFER: 
            return "E_SKATE_UART_MSG_ERR_INVALID_BUFFER";
        case E_SKATE_PS2_ERR_VALUE_READY:
            return "E_SKATE_PS2_ERR_VALUE_READY";
        case E_SKATE_PS2_ERR_INVALID_STATE:
            return "E_SKATE_PS2_ERR_INVALID_STATE";
        default:
            return "?";
    }
}