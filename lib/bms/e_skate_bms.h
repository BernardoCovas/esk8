#ifndef _E_SKATE_BMS_H
#define _E_SKATE_BMS_H


typedef enum e_skate_bms_err_t
{
    E_SKATE_UART_BMS_SUCCESS,
    E_SKATE_UART_BMS_ERR_INVALID_DATA,
    E_SKATE_UART_BMS_ERR_INVALID_LEN,
    E_SKATE_UART_BMS_ERR_WRONG_RESPONSE,
    E_SKATE_UART_BMS_ERR_NO_RESPONSE,
} e_skate_bms_err_t;


e_skate_bms_err_t e_skate_bms_uart_setup();

#endif /* _E_SKATE_BMS_H */
