#ifndef _ESK8_ERR_H
#define _ESK8_ERR_H

#define ESK8_ERRCHECK_DO(X, _do_what) do { esk8_err_t __err_c = (X); if (__err_c) { _do_what; } } while(0)
#define ESK8_ERRCHECK_THROW(X) do { esk8_err_t __err_c = (X); if (__err_c) return __err_c; } while(0)
#define ESK8_ERRCHECK_LOG(tag, X) do { esk8_err_t __err_c = (X); if (__err_c) printf(tag "Got error: %s\n", esk8_err_to_str(__err_c)); } while(0)


typedef enum esk8_err_t
{
    ESK8_OK,
    ESK8_ERR_INVALID_PARAM,
    ESK8_ERR_OOM,
    ESK8_BMS_ERR_INVALID_DATA,
    ESK8_BMS_ERR_INVALID_LEN,
    ESK8_BMS_ERR_WRONG_ADDRESS,           /* Address was not what was expected */
    ESK8_BMS_ERR_WRONG_RESPONSE,          /* Response was not what was expected */
    ESK8_BMS_ERR_NO_RESPONSE,
    ESK8_UART_MSG_ERR_NO_HEADER,
    ESK8_UART_MSG_ERR_INVALID_PLDLEN,     /* Payload was either bigger or smaller than advertized in header */
    ESK8_UART_MSG_ERR_INVALID_CHKSUM,
    ESK8_UART_MSG_ERR_INVALID_BUFFER,
    ESK8_PS2_ERR_VALUE_READY,
    ESK8_PS2_ERR_INVALID_STATE,
    ESK8_PS2_ERR_TIMEOUT,
    ESK8_PS2_ERR_RESP_TIMEOUT,            /* There was no response from device. */
    ESK8_PS2_ERR_MVMT_TIMEOUT,            /* Timeout waiting for mvmt packet */
    ESK8_PS2_ERR_BAD_MVMT,                /* The mvmt signal bit was 0. It is always one. This should cause a ps2 resync / reset */
    ESK8_PS2_ERR_BAD_PCK_STRT,            /* Start bit was not 0 */
    ESK8_PS2_ERR_BAD_PCK_STOP,            /* Stop bit was not 1 */
    ESK8_PS2_ERR_BAD_PCK_PRTY,            /* Parity bit is wrong */
    ESK8_PS2_ERR_BAD_PCK_ACKN,            /* Missing ack bit */
    ESK8_PS2_ERR_NO_ACK,                  /* Device has not ack the received command */
    ESK8_PS2_ERR_RESEND,                  /* Device received the command but did not understand it */
    ESK8_PS2_ERR_ERROR,                   /* Device received the command but there was an error */
    ESK8_BLE_INIT_NOINIT,                 /* Not initialized */
    ESK8_BLE_INIT_REINIT,                 /* Already had been initialized */
    ESK8_BLE_INIT_FAILED,
    ESK8_BLE_INIT_MAXREG,                 /* Maximum number of registed apps reached */
    ESK8_BLE_NOTF_TIMEOUT,
    ESK8_BLE_APP_NOREG,                   /* App not registered */
    ESK8_NVS_NOT_INIT,                    /* NVS was not initialized */
    ESK8_NVS_NOT_AVAILABLE,               /* Could not be initialized in any way. Probably broken */
    ESK8_NVS_FULL,                        /* No space left on NVS */
    ESK8_NVS_NO_SETTINGS,                 /* No settigs stored */
    ESK8_NVS_NO_IDX,                      /* NVS index out of range */
    ESK8_NVS_NO_VAL,                      /* Value not stored */
    ESK8_NVS_WRONG_SIZE,                  /* Stored value returned an unexpected size. Might also mean non existent value */
    ESK8_NVS_ERR_WRITE,                   /* An NVS write opperatin failed. */
    ESK8_BTN_ERR_NOINIT,
    ESK8_BTN_ERR_TIMEOUT,
    ESK8_AUTH_ERR_HASH,                   /* An error occured in the tls module. */
    ESK8_AUTH_ERR_AUTH,                   /* Wrong key supplied.                 */
    ESK8_AUTH_ERR_CONN,                   /* Wrong connection authenticating.    */
    ESK8_ERR_OBRD_NOINIT,
    ESK8_ERR_REMT_NOINIT,
    ESK8_ERR_REMT_REINIT,
    ESK8_ERR_REMT_BAD_STATE,
}
esk8_err_t;


/**
 * String representation of `err`.
 * If the error code is unkown,
 * returns "?".
 */
const char* esk8_err_to_str(

    esk8_err_t err

);

#endif /* _ESK8_ERR_H */
