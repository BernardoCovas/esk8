#ifndef _ESK8_REMOTE_H
#define _ESK8_REMOTE_H

#include <esk8_err.h>
#include <esk8_ps2.h>
#include <esk8_pwm.h>
#include <esk8_btn.h>

#include <stdint.h>


esk8_err_t
esk8_rmt_start(
);

esk8_err_t
esk8_rmt_stop(
);

esk8_err_t
esk8_rmt_incr_speed(
    int incr
);

esk8_err_t
esk8_rmt_await_notif(
);

esk8_err_t
esk8_rmt_connect(
    uint32_t sec
);

void
esk8_rmt_task_ps2(
    void* param
);

void
esk8_rmt_task_btn(
    void* param
);

void
esk8_rmt_task_ble(
    void* param
);


#endif /* _ESK8_REMOTE_H */
