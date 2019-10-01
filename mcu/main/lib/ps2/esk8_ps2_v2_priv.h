#ifndef _ESK8_PS2_PRIV_H
#define _ESK8_PS2_PRIV_H


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
    uint8_t start;
    uint8_t byte;
    uint8_t parity;
    uint8_t stop;
    int     idx;
}
esk8_ps2_frame_t;

typedef struct
{
    esk8_ps2_state_t ps2_state;
    esk8_ps2_frame_t inflight;
    void*            rx_cmd_queue;
    void*            rx_mvt_queue;
    void*            rx_tx_lock;
}
esk8_ps2_hndl_def_t;

int
esk8_ps2_get_bit(
    esk8_ps2_frame_t* frame,
    int               idx
);

void
esk8_ps2_set_bit(
    esk8_ps2_frame_t* frame,
    int               idx,
    int               val
);

void
esk8_ps2_reset_frame(
    esk8_ps2_frame_t* frame,
);

int
esk8_ps2_get_parity(
    uint8_t byte
);


#endif /* _ESK8_PS2_PRIV_H */
