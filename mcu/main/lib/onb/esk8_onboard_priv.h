#ifndef _ESK8_ONBOARD_PRIV_H
#define _ESK8_ONBOARD_PRIV_H

#include <esk8_err.h>
#include <esk8_bms.h>
#include <esk8_onboard.h>


typedef enum
{
    ESK8_RIDE_STATE_STOPPED,
    ESK8_RIDE_STATE_INIT,
    ESK8_RIDE_STATE_RUNNING,
    ESK8_RIDE_STATE_ERR_FATAL,
}
esk8_onboard_state_t;

typedef struct
{
    esk8_err_t           err;
    esk8_onboard_cnfg_t  cnfg;
    esk8_onboard_state_t state;

    uint8_t                 now_speed;
    esk8_bms_status_t*      bms_stat;
    esk8_bms_deep_status_t* bms_deep_stat;

    void* hndl_bms;
    void* hndl_pwm;
    void* hndl_btn;
    void* task_bms;
    void* task_btn;
}
esk8_onboard_t;

extern esk8_onboard_t
esk8_onboard;

void
esk8_onboard_task_bms(
    void* param
);

void
esk8_onboard_task_btn(
    void* param
);

esk8_err_t
esk8_onboard_set_speed(
    uint8_t speed
);


#endif /* _ESK8_ONBOARD_PRIV_H */
