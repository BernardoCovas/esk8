#ifndef _ESK8_PWM_H
#define _ESK8_PWM_H

#include <esk8_err.h>

#include <driver/ledc.h>


typedef struct
{
    ledc_timer_config_t timer_cnfg;
    ledc_channel_config_t channel_cnfg;
} esk8_pwm_cnfg_t_;

typedef struct
{
    int      timer_num;
    uint32_t pwm_freq_hz;
    int      pwm_chnl;
    int      pwm_gpio;

}
esk8_pwm_cnfg_t;

typedef void*
esk8_pwm_hndl_t;

esk8_err_t
esk8_pwm_sgnl_init(
    esk8_pwm_cnfg_t* cnfg,
    esk8_pwm_hndl_t* out_hndl
);

esk8_err_t
esk8_pwm_sgnl_init_from_config_h(
    esk8_pwm_hndl_t* out_hndl
);

esk8_err_t
esk8_pwm_sgnl_set(
    esk8_pwm_hndl_t hndl,
    uint8_t pwm_val
);

esk8_err_t
esk8_pwm_sgnl_stop(
    esk8_pwm_hndl_t hndl
);


#endif /* _ESK8_PWM_H */