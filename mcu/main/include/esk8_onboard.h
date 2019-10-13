#ifndef _ESK8_ONBOARD_H
#define _ESK8_ONBOARD_H

#include <esk8_config.h>
#include <esk8_err.h>
#include <esk8_bms.h>
#include <esk8_btn.h>
#include <esk8_pwm.h>
#include <esk8_auth.h>

#include <stdint.h>


typedef struct
{
    int bms_update_ms;
    int btn_timeout_ms;
    int ps2_timeout_ms;
}
esk8_onboard_cnfg_t;

esk8_err_t
esk8_onboard_start(
    esk8_onboard_cnfg_t* cnfg
);

esk8_err_t
esk8_onboard_stop(
);

esk8_err_t
esk8_onboard_set_speed(
    uint8_t speed
);


#endif /* _ESK8_ONBOARD_H */
