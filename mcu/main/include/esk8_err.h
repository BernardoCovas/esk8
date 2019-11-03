#ifndef _ESK8_ERR_H
#define _ESK8_ERR_H

#define ESK8_ERRCHECK_DO(X, _do_what) do { esk8_err_t __err_c = (X); if (__err_c) { _do_what; } } while(0)
#define ESK8_ERRCHECK_THROW(X) do { esk8_err_t __err_c = (X); if (__err_c) return __err_c; } while(0)
#define ESK8_ERRCHECK_LOG(tag, X) do { esk8_err_t __err_c = (X); if (__err_c) esk8_log_E(tag "Got error: %s\n", esk8_err_to_str(__err_c)); } while(0)


typedef enum esk8_err_t
{
    ESK8_OK,
    ESK8_ERR_INVALID_PARAM,
    ESK8_ERR_OOM,
    ESK8_ERR_BMS_INVALID_DATA,
    ESK8_ERR_BMS_INVALID_LEN,
    ESK8_ERR_BMS_WRONG_ADDRESS,             /* Address was not what was expected */
    ESK8_ERR_BMS_WRONG_RESPONSE,            /* Response was not what was expected */
    ESK8_ERR_BMS_NO_RESPONSE,
    ESK8_ERR_UART_MSG_NO_HEADER,
    ESK8_ERR_UART_MSG_INVALID_PLDLEN,       /* Payload was either bigger or smaller than advertized in header */
    ESK8_ERR_UART_MSG_INVALID_CHKSUM,
    ESK8_ERR_UART_MSG_INVALID_BUFFER,
    ESK8_ERR_PS2_VALUE_READY,
    ESK8_ERR_PS2_INVALID_STATE,
    ESK8_ERR_PS2_TIMEOUT,
    ESK8_ERR_PS2_RESP_TIMEOUT,              /* There was no response from device. */
    ESK8_ERR_PS2_MVMT_TIMEOUT,              /* Timeout waiting for mvmt packet */
    ESK8_ERR_PS2_BAD_MVMT,                  /* The mvmt signal bit was 0. It is always one. This should cause a ps2 resync / reset */
    ESK8_ERR_PS2_BAD_PCK_STRT,              /* Start bit was not 0 */
    ESK8_ERR_PS2_BAD_PCK_STOP,              /* Stop bit was not 1 */
    ESK8_ERR_PS2_BAD_PCK_PRTY,              /* Parity bit is wrong */
    ESK8_ERR_PS2_BAD_PCK_ACKN,              /* Missing ack bit */
    ESK8_ERR_PS2_NO_ACK,                    /* Device has not ack the received command */
    ESK8_ERR_PS2_RESEND,                    /* Device received the command but did not understand it */
    ESK8_ERR_PS2_ERROR,                     /* Device received the command but there was an error */
    ESK8_ERR_BLE_WRITE_BAD_LEN,             /* Ble write op with unexpected size */
    ESK8_ERR_BLE_APPS_INIT_NOINIT,          /* Not initialized */
    ESK8_ERR_BLE_APPS_INIT_REINIT,          /* Already had been initialized */
    ESK8_ERR_BLE_APPS_INIT_FAILED,
    ESK8_ERR_BLE_APPS_INIT_MAXREG,          /* Maximum number of registed apps reached */
    ESK8_ERR_BLE_APPS_NOTF_TIMEOUT,
    ESK8_ERR_BLE_APPS_APP_NOREG,            /* App not registered */
    ESK8_ERR_BLES_APP_CTRL_NO_PERM,         /* Client has no permission */
    ESK8_ERR_BLE_APPC_INIT_NOINIT,          /* Not initialized */
    ESK8_ERR_BLE_APPC_INIT_REINIT,          /* Already had been initialized */
    ESK8_ERR_BLE_APPC_INIT_FAILED,
    ESK8_ERR_BLE_APPC_INIT_MAXREG,          /* Maximum number of registed apps reached */
    ESK8_ERR_BLE_APPC_SCAN_FAILED,          /* Not initialized */
    ESK8_ERR_BLE_APPC_DEV_MAXREG,           /* Maximum number of registed devices reached */
    ESK8_ERR_BLE_APPC_NOTF_TIMEOUT,
    ESK8_ERR_BLE_APPC_APP_NOREG,            /* App not registered */
    ESK8_ERR_NVS_NOT_INIT,                  /* NVS was not initialized */
    ESK8_ERR_NVS_NOT_AVAILABLE,             /* Could not be initialized in any way. Probably broken */
    ESK8_ERR_NVS_FULL,                      /* No space left on NVS */
    ESK8_ERR_NVS_NO_SETTINGS,               /* No settigs stored */
    ESK8_ERR_NVS_NO_IDX,                    /* NVS index out of range */
    ESK8_ERR_NVS_NO_VAL,                    /* Value not stored */
    ESK8_ERR_NVS_WRONG_SIZE,                /* Stored value returned an unexpected size. Might also mean non existent value */
    ESK8_ERR_NVS_ERR_WRITE,                 /* An NVS write opperatin failed. */
    ESK8_ERR_BTN_NOINIT,
    ESK8_ERR_BTN_TIMEOUT,
    ESK8_ERR_AUTH_HASH,                     /* An error occured in the tls module. */
    ESK8_ERR_AUTH_AUTH,                     /* Wrong key supplied.                 */
    ESK8_ERR_AUTH_CONN,                     /* Wrong connection authenticating.    */
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
