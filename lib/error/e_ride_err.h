#ifndef _E_RIDE_ERR_H
#define _E_RIDE_ERR_H


typedef enum e_ride_err_t
{
    E_RIDE_SUCCESS,
    E_RIDE_ERR_INVALID_PARAM,
    E_RIDE_ERR_OOM,
    
    /* E_Skate BMS */
    E_RIDE_BMS_ERR_INVALID_DATA,
    E_RIDE_BMS_ERR_INVALID_LEN,
    E_RIDE_BMS_ERR_WRONG_ADDRESS,
    E_RIDE_BMS_ERR_WRONG_RESPONSE,
    E_RIDE_BMS_ERR_NO_RESPONSE,
    
    /* E_Skate Uart */
    E_RIDE_UART_MSG_ERR_NO_HEADER,
    E_RIDE_UART_MSG_ERR_INVALID_PLDLEN,
    E_RIDE_UART_MSG_ERR_INVALID_CHKSUM,
    E_RIDE_UART_MSG_ERR_INVALID_BUFFER,

    /* E_Skate Ps2 */
    E_RIDE_PS2_ERR_VALUE_READY,
    E_RIDE_PS2_ERR_INVALID_STATE,
    E_RIDE_PS2_ERR_TIMEOUT,

} e_ride_err_t;


/**
 * String representation of `errCode`.
 * If the error code is unkown,
 * returns "?".
 **/
const char* e_ride_err_to_str(

    e_ride_err_t errCode

);

#endif /* _E_RIDE_ERR_H */
