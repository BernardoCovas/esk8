#ifndef _E_SKATE_BMS_H
#define _E_SKATE_BMS_H

#include <e_skate_config.h>
#include <e_skate_uart_esp.h>

#include <driver/uart.h>


typedef enum e_skate_bms_err_t
{
    E_SKATE_BMS_SUCCESS,
    E_SKATE_BMS_ERR_INVALID_DATA,
    E_SKATE_BMS_ERR_INVALID_LEN,
    E_SKATE_BMS_ERR_WRONG_RESPONSE,
    E_SKATE_BMS_ERR_NO_RESPONSE,
} e_skate_bms_err_t;


typedef struct
{
    uint8_t     numBat;
    uint8_t*    batRxPins;
    uint8_t     batTxPin;
    uint32_t    bmsUpdateMs;
    uart_config_t uartConfig;
} e_skate_bms_config_t;


e_skate_bms_err_t e_skate_bms_init_from_config_h(

);


#endif /* _E_SKATE_BMS_H */
