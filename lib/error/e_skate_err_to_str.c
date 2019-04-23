#include <e_skate_err.h>

const static char* E_SKATE_SUCCESS_str                     = "E_SKATE_SUCCESS";
const static char* E_SKATE_ERR_INVALID_PARAM_str           = "E_SKATE_ERR_INVALID_PARAM";
const static char* E_SKATE_BMS_ERR_INVALID_DATA_str        = "E_SKATE_BMS_ERR_INVALID_DATA";
const static char* E_SKATE_BMS_ERR_INVALID_LEN_str         = "E_SKATE_BMS_ERR_INVALID_LEN";
const static char* E_SKATE_BMS_ERR_WRONG_ADDRESS_str       = "E_SKATE_BMS_ERR_WRONG_ADDRESS";
const static char* E_SKATE_BMS_ERR_WRONG_RESPONSE_str      = "E_SKATE_BMS_ERR_WRONG_RESPONSE";
const static char* E_SKATE_BMS_ERR_NO_RESPONSE_str         = "E_SKATE_BMS_ERR_NO_RESPONSE";
const static char* E_SKATE_UART_MSG_ERR_NO_HEADER_str      = "E_SKATE_UART_MSG_ERR_NO_HEADER";
const static char* E_SKATE_UART_MSG_ERR_INVALID_PLDLEN_str = "E_SKATE_UART_MSG_ERR_INVALID_PLDLEN";
const static char* E_SKATE_UART_MSG_ERR_INVALID_CHKSUM_str = "E_SKATE_UART_MSG_ERR_INVALID_CHKSUM";
const static char* E_SKATE_UART_MSG_ERR_INVALID_BUFFER_str = "E_SKATE_UART_MSG_ERR_INVALID_BUFFER";

const char* e_skate_err_to_str(
    e_skate_err_t errCode
)
{
    switch(errCode)
    {
        case E_SKATE_SUCCESS: 
            return E_SKATE_SUCCESS_str;
        case E_SKATE_ERR_INVALID_PARAM: 
            return E_SKATE_ERR_INVALID_PARAM_str;
        case E_SKATE_BMS_ERR_INVALID_DATA: 
            return E_SKATE_BMS_ERR_INVALID_DATA_str;
        case E_SKATE_BMS_ERR_INVALID_LEN: 
            return E_SKATE_BMS_ERR_INVALID_LEN_str;
        case E_SKATE_BMS_ERR_WRONG_ADDRESS: 
            return E_SKATE_BMS_ERR_WRONG_ADDRESS_str;
        case E_SKATE_BMS_ERR_WRONG_RESPONSE: 
            return E_SKATE_BMS_ERR_WRONG_RESPONSE_str;
        case E_SKATE_BMS_ERR_NO_RESPONSE: 
            return E_SKATE_BMS_ERR_NO_RESPONSE_str;
        case E_SKATE_UART_MSG_ERR_NO_HEADER: 
            return E_SKATE_UART_MSG_ERR_NO_HEADER_str;
        case E_SKATE_UART_MSG_ERR_INVALID_PLDLEN: 
            return E_SKATE_UART_MSG_ERR_INVALID_PLDLEN_str;
        case E_SKATE_UART_MSG_ERR_INVALID_CHKSUM: 
            return E_SKATE_UART_MSG_ERR_INVALID_CHKSUM_str;
        case E_SKATE_UART_MSG_ERR_INVALID_BUFFER: 
            return E_SKATE_UART_MSG_ERR_INVALID_BUFFER_str;
        default:
            return "?";
    }
}