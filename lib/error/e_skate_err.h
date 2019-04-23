#ifndef _E_SKATE_ERR_H
#define _E_SKATE_ERR_H


typedef enum e_skate_err_t
{
    E_SKATE_SUCCESS,
    E_SKATE_ERR_INVALID_PARAM,
    
    /* E_Skate BMS */
    E_SKATE_BMS_ERR_INVALID_DATA,
    E_SKATE_BMS_ERR_INVALID_LEN,
    E_SKATE_BMS_ERR_WRONG_ADDRESS,
    E_SKATE_BMS_ERR_WRONG_RESPONSE,
    E_SKATE_BMS_ERR_NO_RESPONSE,
    
    /* E_Skate Uart */
    E_SKATE_UART_MSG_ERR_NO_HEADER,
    E_SKATE_UART_MSG_ERR_INVALID_PLDLEN,
    E_SKATE_UART_MSG_ERR_INVALID_CHKSUM,
    E_SKATE_UART_MSG_ERR_INVALID_BUFFER,
} e_skate_err_t;


/**
 * String representation of `errCode`.
 * If the error code is unkown,
 * returns "?".
 **/
const char* e_skate_err_to_str(
    e_skate_err_t errCode
);

#endif /* _E_SKATE_ERR_H */
