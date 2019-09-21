#ifndef _ESK8_REMOTE_H
#define _ESK8_REMOTE_H

#include <esk8_err.h>

#include <stdint.h>


typedef enum
{
    ESK8_REMOTE_STATE_STOPPED,
    ESK8_REMOTE_STATE_INIT,
    ESK8_REMOTE_STATE_NOT_CONNECTED,
    ESK8_REMOTE_STATE_SEARCHING,
    ESK8_REMOTE_STATE_CONNECTED,
    ESK8_REMOTE_STATE_RUNNING,
} esk8_remote_state_t;

typedef struct
{
    esk8_remote_state_t state;
    uint8_t             speed;

    void*               ble_task;
    void*               ps2_task;
    void*               evt_smph;
} esk8_remote_t;

extern esk8_remote_t
esk8_remote;

esk8_err_t
esk8_remote_start(
);

esk8_err_t
esk8_remote_stop(
);

esk8_err_t
esk8_remote_incr_speed(
    int incr
);

esk8_err_t
esk8_remote_await_notif(
);

esk8_err_t
esk8_remote_connect(
    uint32_t sec
);

void
esk8_remote_task_ps2(
    void* param
);

void
esk8_remote_task_ble(
    void* param
);


#endif /* _ESK8_REMOTE_H */
