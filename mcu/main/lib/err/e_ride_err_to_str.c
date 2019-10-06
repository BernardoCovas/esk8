#include <esk8_err.h>


const char* esk8_err_to_str(
    esk8_err_t errCode
)
{
    switch(errCode)
    {
        case ESK8_OK: return "ESK8_OK";
        case ESK8_ERR_INVALID_PARAM: return "ESK8_ERR_INVALID_PARAM";
        case ESK8_ERR_OOM: return "ESK8_ERR_OOM";
        case ESK8_BMS_ERR_INVALID_DATA: return "ESK8_BMS_ERR_INVALID_DATA";
        case ESK8_BMS_ERR_INVALID_LEN: return "ESK8_BMS_ERR_INVALID_LEN";
        case ESK8_BMS_ERR_WRONG_ADDRESS: return "ESK8_BMS_ERR_WRONG_ADDRESS";
        case ESK8_BMS_ERR_WRONG_RESPONSE: return "ESK8_BMS_ERR_WRONG_RESPONSE";
        case ESK8_BMS_ERR_NO_RESPONSE: return "ESK8_BMS_ERR_NO_RESPONSE";
        case ESK8_UART_MSG_ERR_NO_HEADER: return "ESK8_UART_MSG_ERR_NO_HEADER";
        case ESK8_UART_MSG_ERR_INVALID_PLDLEN: return "ESK8_UART_MSG_ERR_INVALID_PLDLEN";
        case ESK8_UART_MSG_ERR_INVALID_CHKSUM: return "ESK8_UART_MSG_ERR_INVALID_CHKSUM";
        case ESK8_UART_MSG_ERR_INVALID_BUFFER: return "ESK8_UART_MSG_ERR_INVALID_BUFFER";
        case ESK8_PS2_ERR_VALUE_READY: return "ESK8_PS2_ERR_VALUE_READY";
        case ESK8_PS2_ERR_INVALID_STATE: return "ESK8_PS2_ERR_INVALID_STATE";
        case ESK8_PS2_ERR_TIMEOUT: return "ESK8_PS2_ERR_TIMEOUT";
        case ESK8_PS2_ERR_RESP_TIMEOUT: return "ESK8_PS2_ERR_RESP_TIMEOUT";
        case ESK8_PS2_ERR_MVMT_TIMEOUT: return "ESK8_PS2_ERR_MVMT_TIMEOUT";
        case ESK8_PS2_ERR_BAD_MVMT: return "ESK8_PS2_ERR_BAD_MVMT";
        case ESK8_PS2_ERR_BAD_PCK_STRT: return "ESK8_PS2_ERR_BAD_PCK_STRT";
        case ESK8_PS2_ERR_BAD_PCK_STOP: return "ESK8_PS2_ERR_BAD_PCK_STOP";
        case ESK8_PS2_ERR_BAD_PCK_PRTY: return "ESK8_PS2_ERR_BAD_PCK_PRTY";
        case ESK8_PS2_ERR_NO_ACK: return "ESK8_PS2_ERR_NO_ACK";
        case ESK8_PS2_ERR_RESEND: return "ESK8_PS2_ERR_RESEND";
        case ESK8_BLE_INIT_NOINIT: return "ESK8_BLE_INIT_NOINIT";
        case ESK8_BLE_INIT_REINIT: return "ESK8_BLE_INIT_REINIT";
        case ESK8_BLE_INIT_FAILED: return "ESK8_BLE_INIT_FAILED";
        case ESK8_BLE_INIT_MAXREG: return "ESK8_BLE_INIT_MAXREG";
        case ESK8_BLE_NOTF_TIMEOUT: return "ESK8_BLE_NOTF_TIMEOUT";
        case ESK8_BLE_APP_NOREG: return "ESK8_BLE_APP_NOREG";
        case ESK8_NVS_NOT_INIT: return "ESK8_NVS_NOT_INIT";
        case ESK8_NVS_NOT_AVAILABLE: return "ESK8_NVS_NOT_AVAILABLE";
        case ESK8_NVS_FULL: return "ESK8_NVS_FULL";
        case ESK8_NVS_NO_SETTINGS: return "ESK8_NVS_NO_SETTINGS";
        case ESK8_NVS_NO_IDX: return "ESK8_NVS_NO_IDX";
        case ESK8_NVS_NO_VAL: return "ESK8_NVS_NO_VAL";
        case ESK8_NVS_WRONG_SIZE: return "ESK8_NVS_WRONG_SIZE";
        case ESK8_NVS_ERR_WRITE: return "ESK8_NVS_ERR_WRITE";
        case ESK8_BTN_ERR_NOINIT: return "ESK8_BTN_ERR_NOINIT";
        case ESK8_BTN_ERR_TIMEOUT: return "ESK8_BTN_ERR_TIMEOUT";
        case ESK8_AUTH_ERR_HASH: return "ESK8_AUTH_ERR_HASH";
        case ESK8_AUTH_ERR_AUTH: return "ESK8_AUTH_ERR_AUTH";
        case ESK8_AUTH_ERR_CONN: return "ESK8_AUTH_ERR_CONN";
        case ESK8_ERR_OBRD_NOINIT: return "ESK8_ERR_OBRD_NOINIT";
        case ESK8_ERR_REMT_NOINIT: return "ESK8_ERR_REMT_NOINIT";
        case ESK8_ERR_REMT_REINIT: return "ESK8_ERR_REMT_REINIT";
        case ESK8_ERR_REMT_BAD_STATE: return "ESK8_ERR_REMT_BAD_STATE";

        default:
            return "unknown_error";
    }
}