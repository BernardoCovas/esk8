#include <e_ride_err.h>


const char* e_ride_err_to_str(
    e_ride_err_t errCode
)
{
    switch(errCode)
    {
        case E_RIDE_SUCCESS: 
            return "E_RIDE_SUCCESS";
        case E_RIDE_ERR_INVALID_PARAM: 
            return "E_RIDE_ERR_INVALID_PARAM";
        case E_RIDE_ERR_OOM:
            return "E_RIDE_ERR_OOM";
        case E_RIDE_BMS_ERR_INVALID_DATA: 
            return "E_RIDE_BMS_ERR_INVALID_DATA";
        case E_RIDE_BMS_ERR_INVALID_LEN: 
            return "E_RIDE_BMS_ERR_INVALID_LEN";
        case E_RIDE_BMS_ERR_WRONG_ADDRESS: 
            return "E_RIDE_BMS_ERR_WRONG_ADDRESS";
        case E_RIDE_BMS_ERR_WRONG_RESPONSE: 
            return "E_RIDE_BMS_ERR_WRONG_RESPONSE";
        case E_RIDE_BMS_ERR_NO_RESPONSE: 
            return "E_RIDE_BMS_ERR_NO_RESPONSE";
        case E_RIDE_UART_MSG_ERR_NO_HEADER: 
            return "E_RIDE_UART_MSG_ERR_NO_HEADER";
        case E_RIDE_UART_MSG_ERR_INVALID_PLDLEN: 
            return "E_RIDE_UART_MSG_ERR_INVALID_PLDLEN";
        case E_RIDE_UART_MSG_ERR_INVALID_CHKSUM: 
            return "E_RIDE_UART_MSG_ERR_INVALID_CHKSUM";
        case E_RIDE_UART_MSG_ERR_INVALID_BUFFER: 
            return "E_RIDE_UART_MSG_ERR_INVALID_BUFFER";
        case E_RIDE_PS2_ERR_VALUE_READY:
            return "E_RIDE_PS2_ERR_VALUE_READY";
        case E_RIDE_PS2_ERR_INVALID_STATE:
            return "E_RIDE_PS2_ERR_INVALID_STATE";
        case E_RIDE_PS2_ERR_TIMEOUT:
            return "E_RIDE_PS2_ERR_TIMEOUT";
        default:
            return "?";
    }
}