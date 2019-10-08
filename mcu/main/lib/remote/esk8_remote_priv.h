#ifndef _ESK8_REMOTE_PRIV_H
#define _ESK8_REMOTE_PRIV_H


typedef enum
{
    ESK8_REMOTE_STATE_STOPPED,
    ESK8_REMOTE_STATE_INIT,
    ESK8_REMOTE_STATE_NOT_CONNECTED,
    ESK8_REMOTE_STATE_SEARCHING,
    ESK8_REMOTE_STATE_CONNECTED,
    ESK8_REMOTE_STATE_RUNNING,
}
esk8_remote_state_t;

typedef struct
{
    esk8_remote_state_t state;

    void* btn_hndl;
    void* ps2_hndl;
    void* btn_task;
    void* ble_task;
    void* ps2_task;
}
esk8_remote_hndl_def_t;


#endif /* _ESK8_REMOTE_PRIV_H */