#ifndef _ESK8_REMOTE_PRIV_H
#define _ESK8_REMOTE_PRIV_H

#include <esk8_rmt.h>


typedef enum
{
    ESK8_REMOTE_STATE_STOPPED,
    ESK8_REMOTE_STATE_INIT,
    ESK8_REMOTE_STATE_NOT_CONNECTED,
    ESK8_REMOTE_STATE_SEARCHING,
    ESK8_REMOTE_STATE_CONNECTED,
    ESK8_REMOTE_STATE_RUNNING,
}
esk8_rmt_state_t;

typedef struct
{
    esk8_rmt_state_t state;
    int   speed;

    void* hndl_btn;
    void* hndl_ps2;
    void* hndl_pwm;
    void* task_btn;
    void* task_ble;
    void* task_ps2;
}
esk8_rmt_t;

extern esk8_rmt_t
esk8_rmt;

#endif /* _ESK8_REMOTE_PRIV_H */