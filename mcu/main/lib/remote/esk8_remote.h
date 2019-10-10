#ifndef _ESK8_REMOTE_H
#define _ESK8_REMOTE_H

#include <esk8_err.h>
#include <esk8_ps2.h>
#include <esk8_pwm.h>
#include <esk8_btn.h>

#include <stdint.h>


typedef struct
{
    uint8_t dummy;
}
esk8_remote_cnfg_t;

esk8_err_t
esk8_remote_start(
    esk8_remote_cnfg_t* rmt_cnfg,
    esk8_ps2_hndl_t     ps2_hndl,
    esk8_btn_hndl_t     btn_hndl
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
esk8_remote_task_btn(
    void* param
);

void
esk8_remote_task_ble(
    void* param
);


#endif /* _ESK8_REMOTE_H */
