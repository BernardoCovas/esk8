#ifndef _ESK8_ERR_H
#define _ESK8_ERR_H


typedef enum esk8_err_t
{
    ESK8_SUCCESS,
    ESK8_ERR_INVALID_PARAM,
    ESK8_ERR_OOM,
    
    /* E_Skate BMS */
    ESK8_BMS_ERR_INVALID_DATA,
    ESK8_BMS_ERR_INVALID_LEN,
    ESK8_BMS_ERR_WRONG_ADDRESS,           /* Address was not what was expected */
    ESK8_BMS_ERR_WRONG_RESPONSE,          /* Response was not what was expected */
    ESK8_BMS_ERR_NO_RESPONSE,
    
    /* E_Skate Uart */
    ESK8_UART_MSG_ERR_NO_HEADER,
    ESK8_UART_MSG_ERR_INVALID_PLDLEN,     /* Payload was either bigger or smaller than advertized in header */
    ESK8_UART_MSG_ERR_INVALID_CHKSUM,
    ESK8_UART_MSG_ERR_INVALID_BUFFER,

    /* E_Skate Ps2 */
    ESK8_PS2_ERR_VALUE_READY,
    ESK8_PS2_ERR_INVALID_STATE,
    ESK8_PS2_ERR_TIMEOUT,
    ESK8_PS2_ERR_NO_ACK,                  /* Device has not ack the received command */

    /* E_Skate Ble*/
    ESK8_BLE_INIT_NOINIT,                 /* Not initialized */
    ESK8_BLE_INIT_REINIT,                 /* Already had been initialized */
    ESK8_BLE_INIT_FAILED,
    ESK8_BLE_INIT_MAXREG,                 /* Maximum number of registed apps reached */
    ESK8_BLE_NOTF_TIMEOUT,

    /* Esk8 NVS */
    ESK8_NVS_NOT_INIT,                    /* NVS was not initialized */
    ESK8_NVS_NOT_AVAILABLE,               /* Could not be initialized in any way. Probably broken */
    ESK8_NVS_FULL,                        /* No space left on NVS */
    ESK8_NVS_NO_SETTINGS,                 /* No settigs stored */
    ESK8_NVS_WRONG_SIZE                   /* Stored value returned an unexpected size. Might also mean non existent value */
} esk8_err_t;


/**
 * String representation of `errCode`.
 * If the error code is unkown,
 * returns "?".
 */
const char* esk8_err_to_str(

    esk8_err_t errCode

);

#endif /* _ESK8_ERR_H */
