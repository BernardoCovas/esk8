#ifndef _ESK8_PS2_H
#define _ESK8_PS2_H

#include <esk8_err.h>

#include <stdint.h>
#include <stdlib.h>


typedef enum
{
    ESK8_PS2_CMD_RESET                    = 0xFF,
    ESK8_PS2_CMD_RESEND                   = 0xFE,
    ESK8_PS2_CMD_DATA_DISABLE             = 0xF5,
    ESK8_PS2_CMD_DATA_ENABLE              = 0xF4,
    ESK8_PS2_CMD_GET_DEV_ID               = 0xF2,
    ESK8_PS2_CMD_GET_STATUS               = 0xE9,
    ESK8_PS2_CMD_SET_DEFAULT              = 0xF6,
    ESK8_PS2_CMD_SET_SMPL_RATE            = 0xF3,
    ESK8_PS2_CMD_SET_MODE_STREAM          = 0xEA,
    ESK8_PS2_CMD_SET_MODE_REMOTE          = 0xF0,
    ESK8_PS2_CMD_SET_MODE_WRAP            = 0xEE,
    ESK8_PS2_CMD_SET_MODE_NOWRAP          = 0xEC,
    ESK8_PS2_CMD_SET_SCALE_2_1            = 0xE7,
    ESK8_PS2_CMD_SET_SCALE_1_1            = 0xE6,
    ESK8_PS2_CMD_SET_RESOLUTION           = 0xE8,
    ESK8_PS2_CMD_MODE_REMOTE_DATA_READ    = 0xEB,

    ESK8_PS2_RES_ACK                      = 0xFA,
    ESK8_PS2_RES_ERROR                    = 0xFC,
    ESK8_PS2_RES_RESEND                   = 0xFE,
}
esk8_ps2_cmd_t;

typedef struct
{
    esk8_err_t err;
 
    int x;
    int y;
    int lft_btn;
}
esk8_ps2_mvmt_t;

typedef struct
{
    size_t   rx_queue_len;
    uint32_t rx_timeout_ms;
    uint8_t  clock_pin;
    uint8_t  data_pin;
}
esk8_ps2_cnfg_t;

typedef void*
esk8_ps2_hndl_t;

esk8_err_t
esk8_ps2_init(
    esk8_ps2_hndl_t* out_hndl,
    esk8_ps2_cnfg_t* ps2_cnfg
);

esk8_err_t
esk8_ps2_init_from_config_h(
    esk8_ps2_hndl_t* hndl
);

esk8_err_t
esk8_ps2_deinit(
    esk8_ps2_hndl_t hndl
);

void
esk8_ps2_send_byte(
    esk8_ps2_hndl_t hndl,
    uint8_t         byte
);

esk8_err_t
esk8_ps2_send_cmd(
    esk8_ps2_hndl_t hndl,
    esk8_ps2_cmd_t  cmd
);

esk8_err_t
esk8_ps2_await_rsp(
    esk8_ps2_hndl_t hndl,
    uint8_t*        out_byte
);

esk8_err_t
esk8_ps2_await_mvmt(
    esk8_ps2_hndl_t hndl,
    esk8_ps2_mvmt_t* out_mvmt
);

esk8_err_t
esk8_ps2_mvmt_sync(
    esk8_ps2_hndl_t hndl
);


#endif /* _ESK8_PS2_H */
