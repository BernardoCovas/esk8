#ifndef _ESK8_PS2_PRIV_H
#define _ESK8_PS2_PRIV_H

#include <esk8_err.h>
#include <esk8_ps2.h>

#include <stdint.h>


typedef enum
{
    ESK8_PS2_STATE_NONE = 0,
    ESK8_PS2_STATE_SEND,
    ESK8_PS2_STATE_RECV,
    ESK8_PS2_STATE_MVMT
}
esk8_ps2_state_t;

typedef struct
{
    esk8_err_t err;

    uint8_t byte;
    int     idx;
}
esk8_ps2_frame_t;

typedef struct
{
    esk8_err_t err;

    uint8_t idx;
    uint8_t mvmt[3];
}
esk8_ps2_mvmt_frame_t;

typedef struct
{
    esk8_ps2_frame_t      inflight;
    esk8_ps2_mvmt_frame_t mvmt_frame;
    esk8_ps2_state_t      ps2_state;
    esk8_ps2_cnfg_t       ps2_cnfg;
    void*                 rx_queue;
    void*                 mv_queue;
    void*                 tx_lock;
}
esk8_ps2_hndl_def_t;

int
esk8_ps2_get_bit(
    esk8_ps2_frame_t* frame,
    int               idx
);

void
esk8_ps2_set_bit(
    uint8_t* byte,
    int      idx,
    int      val
);

void
esk8_ps2_reset_frame(
    esk8_ps2_frame_t* frame
);

int
esk8_ps2_get_parity(
    uint8_t byte
);


#endif /* _ESK8_PS2_PRIV_H */
