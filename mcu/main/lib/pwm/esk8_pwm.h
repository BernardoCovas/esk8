#ifndef _ESK8_PWM_H
#define _ESK8_PWM_H

#include <esk8_err.h>

#include <driver/ledc.h>

typedef struct
{
    ledc_timer_config_t timer_cnfg;
    ledc_channel_config_t channel_cnfg;
} esk8_pwm_cnfg_t;

esk8_err_t
esk8_pwm_sgnl_init(
    esk8_pwm_cnfg_t* out_cnfg
);

esk8_err_t
esk8_pwm_sgnl_set(
    esk8_pwm_cnfg_t *cnfg,
    uint8_t pwm_val
);

esk8_err_t
esk8_pwm_sgnl_stop(
    esk8_pwm_cnfg_t* cnfg
);


#endif /* _ESK8_PWM_H */