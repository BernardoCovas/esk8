#ifndef _ESK8_ONBOARD_H
#define _ESK8_ONBOARD_H

#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_bms.h>
#include <esk8_btn.h>
#include <esk8_pwm.h>
#include <esk8_auth.h>

#include <stdint.h>


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
    int bms_update_delay_ms;
    int btn_timeout_ms;
    int runtime_timer_group;
    int runtime_timer_idx;
}
esk8_onboard_cnfg_t;

typedef struct
{
    esk8_err_t                err;
    esk8_onboard_cnfg_t       cnfg;
    esk8_onboard_state_t      state;

    uint8_t                   speed;
    esk8_bms_status_t*        bms_stat;
    esk8_bms_deep_status_t*   bms_deep_stat;
    esk8_bms_config_t         bms_cnfg;
    esk8_pwm_config_t         pwm_cnfg;
    esk8_btn_cnfg_t           btn_cnfg;

    void*                     bms_stat_task;
    void*                     btn_stat_task;
}
esk8_onboard_t;

extern esk8_onboard_t esk8_onboard;

esk8_err_t
esk8_onboard_start(
    esk8_onboard_cnfg_t* cnfg
);

esk8_err_t
esk8_onboard_stop(
);

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

#endif /* _ESK8_ONBOARD_H */
